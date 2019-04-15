#include "tracer_bridge.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------------------------
TracerBridge::TracerBridge(std::shared_ptr<opentracing::Tracer> tracer) noexcept
    : tracer_{std::move(tracer)} {}
} // namespace python_bridge_tracer
