#include "../FObjects/FixedRateBondForward.h"
#include "../FObjects/FlatForward.h"
#include "../FObjects/FittedBondDiscountCurve.h"

#include "Serialisation.h"
#include "Deserialiser.h"
#include "Log.h"

#include <fstream>
#include <thread>
#include <chrono>

namespace nq {

    void deserialiseInto( Repo& repo, Deserialiser& in, FittedBondDiscountCurve& obj ) {
        LOG( "deserialising FittedBondDiscountCurve" );

        obj.accuracy << in["accuracy"];
        obj.max_evaluations << in["max_evaluations"];
        obj.simplex_lambda << in["simplex_lambda"];
        obj.max_stationary_state_iterations << in["max_stationary_state_iterations"];
    }

    void deserialiseInto( Repo& repo, Deserialiser& in, FlatForward& obj ) {
        LOG( "deserialising FlatForward" );
        // !TBD ...
    }

    void deserialiseInto( Repo& repo, Deserialiser& in, Forward& obj ) {
        LOG( "deserialising Forward" );

        obj.settlement_days << in["settlement_days"];
        // !TBD: other fields

        // Load dependencies
        auto curves = repo.loadAndBuildObjects( {
                in.get<std::string>( "discount_curve" ),
                in.get<std::string>( "income_discount_curve" ) } );

        obj.discount_curve = std::dynamic_pointer_cast<Curve>(curves[0]);
        obj.income_discount_curve = std::dynamic_pointer_cast<Curve>(curves[1]);
    }

    void deserialiseInto( Repo& repo, Deserialiser& in, FixedRateBondForward& obj ) {
        LOG( "deserialising FixedRateBondForward" );

        deserialiseInto( repo, in, static_cast<Forward &>(obj));

        // Load dependency
        auto bonds = repo.loadAndBuildObjects( { in.get<std::string>( "fixed_coupon_bond" ) } );
        obj.fixed_coupon_bond = std::dynamic_pointer_cast<FixedRateBond>( bonds[0]);
    }

    void deserialiseInto( Repo& repo, Deserialiser& in, FixedRateBond& obj ) {
        LOG( "deserialising FixedRateBond" );
        // !TBD
    }

    template<typename T>
    auto deserialiseAs( Repo& repo, Deserialiser& in ) -> Ptr<T> {
        auto obj = std::make_shared<T>();
        deserialiseInto( repo, in, *obj );
        return obj;
    }

    auto loadAsJson( std::string const& id ) -> nlohmann::json {

        // Open file
        auto path = "repo/" + std::string(id) + ".json";
        std::ifstream f(path );
        if( !f )
            throw std::domain_error("Missing data for " + std::string(id) );

        // read in as JSON
        nlohmann::json json;
        f >> json;

        // Sleep for a bit to simulate datastore access latency
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);

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

    auto deserialise( Repo& repo, std::string const& id ) -> Ptr<FObject> {
        Deserialiser in( loadAsJson( id ) );
        auto type = getTypeFromId( id );

        // Create object of appropriate type and deserialise into it from JSON
        if( type == "FixedRateBondForward" )
            return deserialiseAs<FixedRateBondForward>( repo, in );
        if( type == "FlatForward" )
            return deserialiseAs<FlatForward>( repo, in );
        if( type == "FittedBondDiscountCurve" )
            return deserialiseAs<FittedBondDiscountCurve>( repo, in );
        if( type == "FixedRateBond" )
            return deserialiseAs<FixedRateBond>( repo, in );

        // Polymorphic types: extract derived id and recurse
        if( type == "Curve" )
            return deserialise( repo, in.get<std::string>( "curve" ) );

        throw std::domain_error("Unrecognised type: " + std::string(type) );
    }

}
