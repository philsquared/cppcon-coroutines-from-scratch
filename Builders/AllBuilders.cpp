#include "AllBuilders.h"

#include "../FObjects/Forward.h"
#include "../FObjects/FlatForward.h"
#include "../FObjects/FixedRateBond.h"
#include "../FObjects/FixedRateBondForward.h"
#include "../FObjects/FittedBondDiscountCurve.h"

#include "../Infrastructure/Dependencies.h"

namespace nq {

    template<typename T>
    auto build( Deserialiser in ) -> BuildTask;


    void buildInto( Ptr<Forward> const& obj, Deserialiser const& in, Dependencies& dependencies ) {

        LOG( "building into forward: " << obj.get() );
        obj->settlement_days << in["settlement_days"];

        dependencies
            .require( obj->discount_curve, "discount_curve" )
            .require( obj->income_discount_curve, "income_discount_curve" );
    }

    template<>
    auto build<Forward>( Deserialiser in ) -> BuildTask {
        auto obj = std::make_shared<Forward>();
        Dependencies dependencies(in);
        buildInto( obj, in, dependencies );

        co_await dependencies.resolve();

        LOG( "Resumed" );

        // build phase ...

        co_return obj;
    }

    template<>
    auto build<FixedRateBondForward>( Deserialiser in ) -> BuildTask {
        auto obj = std::make_shared<FixedRateBondForward>();
        Dependencies dependencies(in);
        buildInto( std::static_pointer_cast<Forward>( obj ), in, dependencies );

        dependencies.require( obj->fixed_coupon_bond, "fixed_coupon_bond" );

        co_await dependencies.resolve();

        LOG( "Resumed" );

        // build phase ...

        co_return obj;
    }

    template<>
    auto build<FixedRateBond>( Deserialiser in ) -> BuildTask {
        auto obj = std::make_shared<FixedRateBond>();
        // ...
        co_return obj;
    }

    template<>
    auto build<FlatForward>( Deserialiser in ) -> BuildTask {
        auto obj = std::make_shared<FlatForward>();
        // ...
        co_return obj;
    }

    template<>
    auto build<FittedBondDiscountCurve>( Deserialiser in ) -> BuildTask {
        LOG( "build<FittedBondDiscountCurve>" );
        auto obj = std::make_shared<FittedBondDiscountCurve>();

        obj->accuracy << in["accuracy"];
        obj->max_evaluations << in["max_evaluations"];
        obj->simplex_lambda << in["simplex_lambda"];
        obj->max_stationary_state_iterations << in["max_stationary_state_iterations"];

        co_return obj;
    }

    template<>
    auto build<Curve>( Deserialiser in ) -> BuildTask {
        Ptr<Curve> curve;
        Dependencies dependencies( in );
        dependencies.require( curve, "instance" );

        co_await dependencies.resolve();

        assert( curve.get() != nullptr );

        LOG( "Curve resumed - co_returning the object" );
        co_return curve;
    }


    auto buildNamedObject( std::string_view type, Deserialiser&& in ) -> BuildTask {
        if( type == "FixedRateBondForward" )
            return build<FixedRateBondForward>( std::move(in) );
        if( type == "Forward" )
            return build<Forward>( std::move(in) );
        if( type == "FixedRateBond" )
            return build<FixedRateBond>( std::move(in) );
        if( type == "FlatForward" )
            return build<FlatForward>( std::move(in) );
        if( type == "FittedBondDiscountCurve" )
            return build<FittedBondDiscountCurve>( std::move(in) );
        if( type == "Curve" )
            return build<Curve>( std::move(in) );

        throw std::domain_error( "Unrecognised type: " + std::string(type) );
    }

} // namespace nq