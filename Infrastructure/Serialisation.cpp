#include "../Builders/AllBuilders.h"

#include "Serialisation.h"
#include "Deserialiser.h"

#include "json.hpp"

#include <fstream>
#include <algorithm>
#include <chrono>
#include <thread>

namespace nq {

    namespace {
        auto loadAsJson( std::string const& id ) -> nlohmann::json {

            // Open file
            auto path = "repo/" + id + ".json";
            std::ifstream f( path );
            if( !f )
                throw std::domain_error( "Missing data for " + std::string(id) );

            // read in as JSON
            nlohmann::json json;
            f >> json;

            return json;
        }

        auto getTypeFromId( std::string_view id ) -> std::string_view {
            // Ids are represented as <dir>/<filename-without-ext>,
            // where <dir> is actually the type
            auto slashPos = id.find( '/');
            if( slashPos == std::string_view::npos )
                throw std::domain_error("Ill formed id - no '/'' found");
            return id.substr( 0, slashPos );
        }
    }

    auto deserialiseAll( std::vector<std::string> ids ) -> LoadedObjectGenerator {

        // This part simulates retrieving the data from a remote datastore
        std::vector<Deserialiser> deserialisers;
        deserialisers.reserve( ids.size() );

        for( auto const& id : ids ) {
            LOG( "loading: " << id );
            deserialisers.emplace_back( loadAsJson( id ) );
        }

        // Sleep for a bit to simulate datastore access latency
        using namespace std::chrono_literals;
        std::this_thread::sleep_for( 10ms );

        // ---

        // We'll now iterate the raw data and start building the objects,
        // yielding partially "built" objects as we go
        for( size_t i = 0; i < ids.size(); ++i ) {
            auto const& id = ids[i];
            LOG( "[" << i << "] Building: " << id );
            auto task = buildNamedObject( getTypeFromId( id ), std::move( deserialisers[i] ));
            LOG( "got build task for " << id );

            task.setId( id );
            co_yield std::move( task );
        }
    }
}
