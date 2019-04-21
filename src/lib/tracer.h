#pragma once

#include <Python.h>

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// setupTracerClass
//--------------------------------------------------------------------------------------------------
bool setupTracerClass(PyObject* module) noexcept;
} // namespace python_bridge_tracer
