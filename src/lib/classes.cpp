#include "python_bridge_tracer/module.h"

#include "tracer.h"
#include "span_context.h"
#include "span.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// setupClasses
//--------------------------------------------------------------------------------------------------
bool setupClasses(PyObject* module) noexcept {
  if(!setupTracerClass(module)) {
    return false;
  }
  if (!setupSpanContextClass(module)) {
    return false;
  }
  return setupSpanClass(module);
}
} // namespace python_bridge_tracer
