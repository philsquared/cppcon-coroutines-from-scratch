#ifndef NANOQUANT_SERIALISATION_H
#define NANOQUANT_SERIALISATION_H

#include <span>
#include <vector>
#include <string>
#include <optional>

#include "BuildTask.h"

namespace nq {

    class LoadedObjectGenerator : TaskTracker {
    public:
        struct promise_type {
            std::optional<BuildTask> buildTask;

            // Coroutine invoked methods
            auto get_return_object()            { return LoadedObjectGenerator{ handle::from_promise( *this ) }; }
            auto initial_suspend() const        { return std::suspend_never{}; }
            auto final_suspend() const noexcept { return std::suspend_always{}; }
            void unhandled_exception() const    { std::terminate(); }
            auto yield_value( BuildTask&& buildTask ) {
                this->buildTask.emplace( std::move(buildTask) );
                return std::suspend_always();
            }
            void return_void() {}
        };
    private:
        using handle = std::coroutine_handle<promise_type>;
        handle coroutineHandle;

    public:
        explicit LoadedObjectGenerator( auto coroutineHandle )
        : coroutineHandle( coroutineHandle ) {
            assert( coroutineHandle );
        }
        ~LoadedObjectGenerator() {
            LOG( "~LoadedObjectGenerator: " << this );
            if( coroutineHandle ) {
                LOG( "~Handle: " << coroutineHandle.address() );
                coroutineHandle.destroy();
            }
        }

        auto hasValue() const {
            return coroutineHandle.promise().buildTask.has_value();
        }
        auto currentTask() const -> BuildTask& {
            assert( hasValue() );
            return *coroutineHandle.promise().buildTask;
        }

        void next() {
            if( !coroutineHandle.done() ) {
                LOG( "Resuming deserialisation task" );
                coroutineHandle.resume();
                if( coroutineHandle.done() )
                    coroutineHandle.promise().buildTask.reset();
            }
        }
    };

    auto deserialiseAll( std::vector<std::string> ids ) -> LoadedObjectGenerator;
}

#endif //NANOQUANT_SERIALISATION_H
