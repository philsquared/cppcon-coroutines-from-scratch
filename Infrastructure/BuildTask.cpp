#include "BuildTask.h"

namespace nq {

    auto BuildTask::resume() const -> Ptr<FObject> {
        LOG( "potentially resuming task for " << getId() );
        if( !handle.done() ) {
            LOG( " - resuming" );
            handle.resume();
        }
        if( handle.done() ) {
            assert( handle.data().obj );
            return handle.data().obj;
        }
        else
            return {};
    }

    auto BuildTask::operator <=> ( BuildTask const& other ) const noexcept -> std::weak_ordering {
        auto cmp = handle.data().id.compare( other.handle.data().id );
        if( cmp == 0 )
            return std::weak_ordering::equivalent;
        else if( cmp < 0 )
            return std::weak_ordering::less;
        else
            return std::weak_ordering::greater;
    }

} // namespace nq
