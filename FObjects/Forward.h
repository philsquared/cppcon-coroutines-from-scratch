//
// Created by Phil Nash on 21/04/2022.
//

#ifndef NANOQUANT_FORWARD_H
#define NANOQUANT_FORWARD_H

#include "FObject.h"
#include "Curve.h"
#include "../SimpleTypes/DayCounter.h"
#include "../SimpleTypes/Calendar.h"
#include "../Enums/BusinessDayConvention.h"
#include "../SimpleTypes/Payoff.h"
#include "../SimpleTypes/Date.h"
#include "../Infrastructure/Ptr.h"

namespace nq {

    struct Forward : FObject {
        DayCounter day_counter;
        Calendar calendar;
        BusinessDayConvention business_day_convention;
        int settlement_days;
        Payoff payoff;
        Date value_date;
        Date maturity_date;
        Ptr<Curve> discount_curve;
        Ptr<Curve> income_discount_curve;
    };

} // namespace nq

#endif //NANOQUANT_FORWARD_H
