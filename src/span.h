#pragma once

#include <Python.h>

#include <opentracing/span.h>
#include <opentracing/tracer.h>

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// makeSpan
//--------------------------------------------------------------------------------------------------
PyObject* makeSpan(opentracing::Tracer& tracer,
                    opentracing::string_view operation_name, PyObject* parent,
                    PyObject* references, PyObject* tags,
                    double start_time) noexcept;

//--------------------------------------------------------------------------------------------------
// setupSpanClass
//--------------------------------------------------------------------------------------------------
void setupSpanClass(PyObject* module) noexcept;
} // namespace python_bridge_tracer
