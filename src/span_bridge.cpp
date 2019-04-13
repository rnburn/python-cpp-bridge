#include "span_bridge.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------------------------
SpanBridge::SpanBridge(std::shared_ptr<opentracing::Span> span) noexcept
    : span_{std::move(span)} {}
}  // namespace python_bridge_tracer
