#include "Repo.h"
#include "Serialisation.h"

#include <iterator>

namespace nq {

    auto Repo::requireObject( std::string const& id ) -> Ptr<FObject> {

        // If it's in the cache, return it
        if( auto it = cache.find( id ); it != cache.end() )
            return it->second;

        // Otherwise mark as required for loading
        if( std::find( toLoad.begin(), toLoad.end(), id ) == toLoad.end() )
            toLoad.push_back( id);
        return {};
    }

    auto Repo::resolveDependencies( BuildTask& task ) -> bool {
        // resolve any dependencies we already have (and note any we don't)
        bool hasUnmetDependencies = false;
        for( auto &&dep: task.getDependencies() ) {
            if( !dep.isMet() ) {
                auto depId = dep.id;
                LOG( "dep: " << depId );
                if( auto obj = requireObject( depId ) ) {
                    LOG( " - setting cached dependency" );
                    dep.supplyDependency( obj );
                } else {
                    LOG( "added dependency: " << depId );
                    hasUnmetDependencies = true;
                }
            }
        }

        if( !hasUnmetDependencies ) {
            auto id = task.getId();
            LOG( "dependent task " << id << " is now ready to resume" );
            if( auto obj = task.resume() ) {
                LOG( "dependent task " << id << " is now built" );
                cache.try_emplace( id, obj );
                return true;
            }
        }
        return false;
    }

    auto Repo::loadAndBuildObjects( std::initializer_list<std::string> ids ) -> std::vector<Ptr<FObject>> {

        std::for_each( ids.begin(), ids.end(), [this]( auto&& id ){ requireObject(id); } );

        do {
            while( !toLoad.empty()) {
                auto buildTasks = deserialiseAll( toLoad );

                LOG( "loaded" );
                toLoad.clear();

                for( ; buildTasks.hasValue(); buildTasks.next() ) {
                    auto task = std::move( buildTasks.currentTask() );
                    LOG( "Build loop for: " << task.getId() );

                    if( !resolveDependencies( task ) )
                        incompleteTasks.emplace_back( std::move( task ) );
                }

            } // end while

            for( size_t i=incompleteTasks.size(); i > 0; --i ) {
                if( resolveDependencies( incompleteTasks[i - 1] ) ) {
                    incompleteTasks.erase( incompleteTasks.begin() + (i-1), incompleteTasks.end() );
                }
            }
        }
        while( !toLoad.empty() );

        assert( incompleteTasks.empty() );
        assert( nq::TaskTracker::diagnosticCount == 0 );

        // At this point all objects should be loaded and built
        std::vector<Ptr<FObject>> objects;
        objects.reserve( ids.size() );
        std::transform(ids.begin(), ids.end(), std::back_inserter(objects),
                               [this]( std::string const& id ) { return cache[id]; });

        LOG( "returning objects");
        return objects;
    }

} // namespace nq
