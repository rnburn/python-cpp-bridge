#pragma once

#include <memory>

#include <Python.h>

#include "span_context_bridge.h"

#include "opentracing/span.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// makeSpanContext
//--------------------------------------------------------------------------------------------------
PyObject* makeSpanContext(
    std::unique_ptr<SpanContextBridge>&& span_context_bridge) noexcept;

PyObject* makeSpanContext(std::unique_ptr<const opentracing::SpanContext>&& span_context) noexcept;

PyObject* makeSpanContext(std::shared_ptr<const opentracing::Span>& span) noexcept;

//--------------------------------------------------------------------------------------------------
// isSpanContext
//--------------------------------------------------------------------------------------------------
bool isSpanContext(PyObject* object) noexcept;

//--------------------------------------------------------------------------------------------------
// getSpanContext
//--------------------------------------------------------------------------------------------------
SpanContextBridge getSpanContext(PyObject* object) noexcept;

//--------------------------------------------------------------------------------------------------
// setupSpanContextClass
//--------------------------------------------------------------------------------------------------
bool setupSpanContextClass(PyObject* module) noexcept;
} // namespace python_bridge_tracer
