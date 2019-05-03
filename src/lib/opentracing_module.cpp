#include "opentracing_module.h"

#include "utility.h"

namespace python_bridge_tracer {
PyObject* getThreadLocalScopeManager() noexcept {
  auto scope_managers = PyImport_ImportModule("scope_managers.scope_managers");
  if (scope_managers == nullptr) {
    return nullptr;
  }
  auto on_exit = finally([scope_managers] {
      Py_DECREF(scope_managers);
  });
  // Maybe _PyObject_CallMethodIdObjArgs?
  return nullptr;
}
} // namespace python_bridge_tracer
