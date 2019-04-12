#pragma once

#include <Python.h>

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// setupTracerClass
//--------------------------------------------------------------------------------------------------
void setupTracerClass(PyObject* module_dict) noexcept;
} // namespace python_bridge_tracer
