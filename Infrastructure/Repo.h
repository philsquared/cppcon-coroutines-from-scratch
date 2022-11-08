#ifndef NANOQUANT_REPO_H
#define NANOQUANT_REPO_H

#include <unordered_map>
#include <string>
#include <vector>

#include "../FObjects/FObject.h"
#include "BuildTask.h"
#include "Ptr.h"

namespace nq {

    class Repo {
        std::unordered_map<std::string, Ptr<FObject>> cache;
        std::vector<std::string> toLoad;
        std::vector<BuildTask> incompleteTasks;

        auto requireObject( std::string const& id ) -> Ptr<FObject>;
        auto resolveDependencies( BuildTask& task ) -> bool;
    public:
        auto loadAndBuildObjects( std::initializer_list<std::string> ids ) -> std::vector<Ptr<FObject>>;
    };

} // namespace nq

#endif //NANOQUANT_REPO_H
