#include <memory>

#ifndef NANOQUANT_PTR_H
#define NANOQUANT_PTR_H

namespace nq {

    template<typename T>
    using Ptr = std::shared_ptr<T>;

} // namespace nq

#endif //NANOQUANT_PTR_H
