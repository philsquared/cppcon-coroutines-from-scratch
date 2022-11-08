#ifndef NANOQUANT_BUILDTASK_H
#define NANOQUANT_BUILDTASK_H

#include "../FObjects/FObject.h"

#include "Deserialiser.h"
#include "Dependency.h"
#include "Ptr.h"
#include "Log.h"
#include "Dependencies.h"
#include "Continuations.h"

#include <span>
#include <string>

namespace nq {

    struct DependencyAwaiter {
        bool shouldSuspend;

        bool ready() const noexcept {
            return !shouldSuspend;
        }
        bool suspend( FrameHandle<> handle ) noexcept {
            LOG("suspending");
            return true;
        }
        void resume() noexcept {
            LOG("resuming" );
        }
    };

    class BuildTask {
    public:

        struct Data {
            std::string id;
            std::vector<Dependency> dependencies;
            Ptr<FObject> obj = nullptr;

            auto get_return_object()                { return BuildTask( Handle::from_data( *this ) ); }
            auto initial_suspend() const -> bool    { return false; }
            void return_value( Ptr<FObject>&& new_obj ) {
                LOG( "Setting return object for " << id << " (" << new_obj.get() << ")" );
                obj = std::move(new_obj);
            }

            auto await_transform( Dependencies&& dependencies ) -> DependencyAwaiter {
                this->dependencies = std::move( dependencies.unmetDependencies );
                return DependencyAwaiter{ !this->dependencies.empty() };
            }
        };


        using Handle = FrameHandle<Data>;
        Handle handle;
        auto data() const -> Data& {
            assert( handle );
            return handle.data();
        }

        explicit BuildTask( Handle handle ) noexcept
        : handle( handle ) {
            assert( handle );
        }
        ~BuildTask() {
            destroyFrame();
        }
        void destroyFrame() {
            if( handle ) {
                LOG( "(" << this << ") destroying frame Handle: " << handle.address() );
                handle.destroy();
            }
            else
                LOG( "(" << this << ") empty/ moved-from" );
        }

        // move-only
        BuildTask( BuildTask&& other ) noexcept {
            assert( other.handle );
            std::swap( handle, other.handle );
        }
        auto operator=( BuildTask&& other ) noexcept -> BuildTask& {
            destroyFrame();
            handle = other.handle;
            other.handle = Handle();
            return *this;
        }

        void setId( std::string const& id ) {
            handle.data().id = id;
        }
        auto getId() const {
            return handle.data().id;
        }

        auto getDependencies() const -> std::span<Dependency> {
            return handle.data().dependencies;
        }

        auto operator <=> ( BuildTask const& other ) const noexcept -> std::weak_ordering;

        auto resume() const -> Ptr<FObject>;
    };


} // namespace nq

#endif //NANOQUANT_BUILDTASK_H
