#include "Dependencies.h"

#include "BuildTask.h"

namespace nq {

    void Dependencies::Awaiter::await_suspend( std::coroutine_handle<BuildTask::promise_type> awaitingCoroutine ) noexcept {
        LOG("await_suspend");
        id = awaitingCoroutine.promise().id;

        assert( awaitingCoroutine.promise().dependencies.empty() );

        awaitingCoroutine.promise().dependencies = dependencies;
    }

    auto Dependencies::resolve() -> Dependencies::Awaiter {
        return Awaiter( unmetDependencies );
    }

} // namespace nq