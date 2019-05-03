#pragma once

#include <Python.h>

namespace python_bridge_tracer {
PyObject* getThreadLocalScopeManager() noexcept;
} // namespace python_bridge_tracer
