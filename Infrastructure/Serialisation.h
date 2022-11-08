#ifndef NANOQUANT_SERIALISATION_H
#define NANOQUANT_SERIALISATION_H

#include <span>
#include <vector>
#include <string>
#include <optional>

#include "BuildTask.h"

namespace nq {

    auto deserialiseAll( std::vector<std::string> ids ) -> std::vector<BuildTask>;
}

#endif //NANOQUANT_SERIALISATION_H
