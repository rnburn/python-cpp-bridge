#include "python_bridge_tracer/module.h"

#include "utility.h"

namespace python_bridge_tracer {
PyObject* getThreadLocalScopeManager() noexcept {
  auto scope_managers = PyImport_ImportModule("opentracing.scope_managers");
  if (scope_managers == nullptr) {
    return nullptr;
  }
  auto cleanup_scope_managers = finally([scope_managers] {
      Py_DECREF(scope_managers);
  });

  auto scope_manager =
      PyObject_GetAttrString(scope_managers, "ThreadLocalScopeManager");
  if (scope_manager == nullptr) {
    return nullptr;
  }
  auto cleanup_scope_manager = finally([scope_manager] {
      Py_DECREF(scope_manager);
  });
  return PyObject_CallObject(scope_manager, nullptr);
}
} // namespace python_bridge_tracer
