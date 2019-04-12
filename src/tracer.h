#pragma once

#include <Python.h>

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// loadTracer
//--------------------------------------------------------------------------------------------------
PyObject* loadTracer(PyObject* self, PyObject* args, PyObject* keywords) noexcept;

//--------------------------------------------------------------------------------------------------
// setupTracerClass
//--------------------------------------------------------------------------------------------------
void setupTracerClass(PyObject* module_dict) noexcept;
} // namespace python_bridge_tracer
