#pragma once

#include <Python.h>

#include "span_bridge.h"
#include "span_context_bridge.h"

#include "opentracing/span.h"
#include "opentracing/tracer.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// makeSpan
//--------------------------------------------------------------------------------------------------
PyObject* makeSpan(std::unique_ptr<SpanBridge>&& span_bridge, PyObject* tracer) noexcept;

//--------------------------------------------------------------------------------------------------
// setupSpanClass
//--------------------------------------------------------------------------------------------------
bool setupSpanClass(PyObject* module) noexcept;

//--------------------------------------------------------------------------------------------------
// isSpan
//--------------------------------------------------------------------------------------------------
bool isSpan(PyObject* object) noexcept;

//--------------------------------------------------------------------------------------------------
// getSpanContext
//--------------------------------------------------------------------------------------------------
SpanContextBridge getSpanContextFromSpan(PyObject* object) noexcept;
} // namespace python_bridge_tracer
