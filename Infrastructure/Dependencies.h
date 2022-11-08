#ifndef NANOQUANT_DEPENDENCIES_H
#define NANOQUANT_DEPENDENCIES_H

#include "Deserialiser.h"
#include "Dependency.h"
#include "Log.h"
#include "BuildTask.h"

#include <vector>
#include <coroutine>

namespace nq {

    class Dependencies {

        std::vector<Dependency> unmetDependencies;
        Deserialiser const& in;

    public:
        Dependencies( Deserialiser const& in ) : in(in) {}

        template<typename T>
        auto require( Ptr<T>& field, std::string const& fieldName ) -> Dependencies& {
            auto setter = [&field]( Ptr<FObject> const& dep ) {
                field = std::dynamic_pointer_cast<T>( dep );
            };
            auto dependentId = in.get<std::string>( fieldName );
            LOG_VAR( dependentId );
            unmetDependencies.emplace_back( std::move( dependentId), std::move( setter ) );
            return *this;
        }

        struct Awaiter;

        auto resolve() -> Awaiter;
    };

    struct Dependencies::Awaiter {
        std::span<Dependency> dependencies;
        std::string_view id;

        explicit Awaiter( std::span<Dependency> dependencies ) noexcept
        : dependencies( dependencies )
        {}
        void operator=( Awaiter&& ) = delete;

        bool await_ready() const noexcept {
            return dependencies.empty();
        }
        void await_suspend( std::coroutine_handle<BuildTask::promise_type> newAwaitingCoroutine ) noexcept;

        void await_resume() noexcept {
            LOG("Dependencies::Awaiter - await_resume for id: " << id );
        }
    };

} // namespace nq

#endif //NANOQUANT_DEPENDENCIES_H
