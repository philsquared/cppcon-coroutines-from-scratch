#ifndef NANOQUANT_DEPENDENCY_H
#define NANOQUANT_DEPENDENCY_H

#include "Ptr.h"

#include "../FObjects/FObject.h"

#include <functional>

namespace nq {

    class Dependency {
        using SetterFunction = std::function<void(Ptr<FObject> const &)>;
        SetterFunction setter;
    public:
        std::string id;
        Dependency( std::string&& id, SetterFunction&& setter )
        : id( std::move(id ) ),
          setter( std::move( setter ) )
        {}

        auto isMet() const -> bool {
            return !setter;
        }

        void supplyDependency( Ptr<FObject> const& obj ) {
            assert( !isMet() );
            setter( obj );
            setter = SetterFunction();
        }
    };

} // namespace nq

#endif //NANOQUANT_DEPENDENCY_H
