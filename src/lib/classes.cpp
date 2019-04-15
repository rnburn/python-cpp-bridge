#include "python_bridge_tracer/module.h"

#include "tracer.h"
#include "span.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// setupClasses
//--------------------------------------------------------------------------------------------------
bool setupClasses(PyObject* module) noexcept {
  setupTracerClass(module);
  setupSpanClass(module);
  return true;
}
} // namespace python_bridge_tracer
