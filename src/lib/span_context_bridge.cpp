#include "span_context_bridge.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------------------------
SpanContextBridge::SpanContextBridge(
    const std::shared_ptr<const opentracing::Span>& span) noexcept
    : span_{span} {}

SpanContextBridge::SpanContextBridge(
    std::unique_ptr<const opentracing::SpanContext>&& span_context) noexcept
    : span_context_{std::move(span_context)} {}

//--------------------------------------------------------------------------------------------------
// span_context
//--------------------------------------------------------------------------------------------------
const opentracing::SpanContext& SpanContextBridge::span_context() const noexcept {
  if (span_ != nullptr) {
    return span_->context();
  }
  return *span_context_;
}
} // namespace python_bridge_tracer
