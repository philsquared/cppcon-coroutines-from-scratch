#ifndef NANOQUANT_REPO_H
#define NANOQUANT_REPO_H

#include "../FObjects/FObject.h"
#include "Ptr.h"

#include <unordered_map>
#include <span>
#include <string_view>
#include <string>
#include <vector>

namespace nq {

class Repo {
    std::unordered_map<std::string, Ptr<FObject>> cache;

    auto load( std::string const& id ) -> Ptr<FObject>;

public:
    auto loadAndBuildObjects( std::initializer_list<std::string> ids) -> std::vector<Ptr<FObject>>;
};

} // namespace nq

#endif //NANOQUANT_REPO_H
