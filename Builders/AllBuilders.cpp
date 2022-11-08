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
        auto frame = makeFrame<BuildTask::Data> ( [in=std::move(in)](Frame<BuildTask::Data>::ContinuationMgr co) mutable {
            auto obj = std::make_shared<Forward>();
            Dependencies dependencies(in);
            buildInto( obj, in, dependencies );

            return co.await( std::move( dependencies ) )
                .then( [obj = std::move( obj )](Frame<BuildTask::Data>::ContinuationMgr co) {
                LOG( "Resumed" );

                // build phase ...

                co.return_( obj );
            } );
        } );
        return frame->data.get_return_object();
    }

    template<>
    auto build<FixedRateBondForward>( Deserialiser in ) -> BuildTask {
        auto frame = makeFrame<BuildTask::Data> ( [in=std::move(in)](auto co) mutable {

            auto obj = std::make_shared<FixedRateBondForward>();
            Dependencies dependencies(in);
            buildInto( std::static_pointer_cast<Forward>( obj ), in, dependencies );

            dependencies.require( obj->fixed_coupon_bond, "fixed_coupon_bond" );

            return co.await( std::move(dependencies) ).then( [obj=std::move(obj)](auto co) {
                    LOG( "Resumed" );

                    // build phase ...

                    co.return_( obj );
                } );
        } );
        return frame->data.get_return_object();
    }

    template<>
    auto build<FixedRateBond>( Deserialiser in ) -> BuildTask {
        auto frame = makeFrame<BuildTask::Data> ( [in=std::move(in)](auto co) mutable {
            auto obj = std::make_shared<FixedRateBond>();
            // ...
            co.return_( obj );
        } );
        return frame->data.get_return_object();
    }

    template<>
    auto build<FlatForward>( Deserialiser in ) -> BuildTask {
        auto frame = makeFrame<BuildTask::Data> ( [in=std::move(in)](auto co) mutable {
            auto obj = std::make_shared<FlatForward>();
            // ...
            co.return_( obj );
        } );
        return frame->data.get_return_object();
    }

    template<>
    auto build<FittedBondDiscountCurve>( Deserialiser in ) -> BuildTask {
        auto frame = makeFrame<BuildTask::Data> ( [in=std::move(in)](auto co) mutable {
            LOG( "build<FittedBondDiscountCurve>" );
            auto obj = std::make_shared<FittedBondDiscountCurve>();

            obj->accuracy << in["accuracy"];
            obj->max_evaluations << in["max_evaluations"];
            obj->simplex_lambda << in["simplex_lambda"];
            obj->max_stationary_state_iterations << in["max_stationary_state_iterations"];

            co.return_( obj );
        } );
        return frame->data.get_return_object();
    }

    template<>
    auto build<Curve>( Deserialiser in ) -> BuildTask {
        auto frame = makeFrame<BuildTask::Data> ( [in=std::move(in)](auto co) mutable {
            struct CurveHolder {
                Ptr<Curve> curve;
            };
            auto curveHolder = std::make_shared<CurveHolder>();
            Dependencies dependencies( in );
            dependencies.require( curveHolder->curve, "instance" );

            return co.await( std::move(dependencies) ).then( [curveHolder=std::move(curveHolder)](auto co) {
                assert( curveHolder->curve.get() != nullptr );

                LOG( "Curve resumed - returning the object" );
                co.return_( curveHolder->curve );
            } );
        } );
        return frame->data.get_return_object();
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