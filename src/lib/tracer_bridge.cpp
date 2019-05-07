#include "tracer_bridge.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------------------------
TracerBridge::TracerBridge(std::shared_ptr<opentracing::Tracer> tracer) noexcept
    : tracer_{std::move(tracer)} {}

//--------------------------------------------------------------------------------------------------
// makeSpan
//--------------------------------------------------------------------------------------------------
std::unique_ptr<SpanBridge> TracerBridge::makeSpan(
    opentracing::string_view operation_name, PyObject* parent,
    PyObject* references, PyObject* tags, double start_time) noexcept {
  (void)parent;
  (void)references;
  (void)tags;
  (void)start_time;
  return std::unique_ptr<SpanBridge>{
      new SpanBridge{tracer_->StartSpan(operation_name)}};
}
} // namespace python_bridge_tracer
