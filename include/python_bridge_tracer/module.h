#pragma once

#ifndef PYTHON_BRIDGE_TRACER_MODULE
#define PYTHON_BRIDGE_TRACER_MODULE "bridge_tracer"
#endif

#include <Python.h>
#include <opentracing/tracer.h>

namespace python_bridge_tracer {
PyObject* makeTracer(std::shared_ptr<opentracing::Tracer> tracer, PyObject* scope_manager) noexcept;

PyObject* getThreadLocalScopeManager() noexcept;

bool setupClasses(PyObject* module) noexcept;
} // namespace python_bridge_tracer
