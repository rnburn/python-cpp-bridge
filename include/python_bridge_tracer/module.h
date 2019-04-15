#pragma once

#include <Python.h>
#include <opentracing/tracer.h>

namespace python_bridge_tracer {
PyObject* makeTracer(std::shared_ptr<opentracing::Tracer> tracer) noexcept;

bool setupClasses(PyObject* module) noexcept;
} // namespace python_bridge_tracer
