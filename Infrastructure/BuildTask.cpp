#include "BuildTask.h"

namespace nq {
    auto BuildTask::resume() const -> Ptr<FObject> {
        LOG( "potentially resuming task for " << getId() );
        if( !handle.done() ) {
            LOG( " - resuming" );
            handle.resume();
            if( !handle.done())
                return {};
        }
        assert( promise().obj );
        return promise().obj;
    }

} // namespace nq
