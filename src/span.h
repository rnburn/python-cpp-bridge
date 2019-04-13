#pragma once

#include <Python.h>

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// setupSpanClass
//--------------------------------------------------------------------------------------------------
void setupSpanClass(PyObject* module_dict) noexcept;
} // namespace python_bridge_tracer
