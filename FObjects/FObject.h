//
// Common base class for all "Financial Objects"
//

#ifndef NANOQUANT_FOBJECT_H
#define NANOQUANT_FOBJECT_H

namespace nq {

struct FObject {
    virtual ~FObject() = default;
};

} // namespace nq

#endif //NANOQUANT_FOBJECT_H
