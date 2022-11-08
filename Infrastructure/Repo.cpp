#include "Repo.h"
#include "Serialisation.h"

namespace nq {

    auto Repo::load( std::string const& id ) -> Ptr<FObject> {
        auto object = deserialise( *this, id );
        cache.try_emplace( id, object );
        return object;
    }

    auto Repo::loadAndBuildObjects( std::initializer_list<std::string> ids) -> std::vector<Ptr<FObject>> {
        std::vector<Ptr<FObject>> objects;
        objects.reserve(ids.size());

        for (auto &&id: ids) {
            auto it = cache.find(id);
            if ( it == cache.end() ) {
                auto obj = load( id );
                objects.push_back( obj );
            }
            else
                objects.push_back( it->second );
        }

        return objects;
    }

} // namespace nq
