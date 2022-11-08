//
// Created by Phil Nash on 21/04/2022.
//

#ifndef NANOQUANT_FIXEDRATEBONDFORWARD_H
#define NANOQUANT_FIXEDRATEBONDFORWARD_H

#include "Forward.h"
#include "FixedRateBond.h"

namespace nq {

    struct FixedRateBondForward : Forward {
        Ptr<FixedRateBond> fixed_coupon_bond;
    };

} // namespace nq

#endif //NANOQUANT_FIXEDRATEBONDFORWARD_H
