#include "span.h"

#include "span_bridge.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// SpanObject
//--------------------------------------------------------------------------------------------------
namespace {
struct SpanObject {
  PyObject_HEAD
  SpanBridge* span_bridge;
};
} // namespace

//--------------------------------------------------------------------------------------------------
// setupSpanClass
//--------------------------------------------------------------------------------------------------
void setupSpanClass(PyObject* module_dict) noexcept {
}
} // namespace python_bridge_tracer
