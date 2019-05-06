#pragma once

#include <memory>

#include <Python.h>

#include "opentracing/span.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// makeSpanContext
//--------------------------------------------------------------------------------------------------
PyObject* makeSpanContext(std::unique_ptr<const opentracing::SpanContext>&& span_context) noexcept;

PyObject* makeSpanContext(std::shared_ptr<const opentracing::Span>& span) noexcept;

//--------------------------------------------------------------------------------------------------
// getSpanContext
//--------------------------------------------------------------------------------------------------
const opentracing::SpanContext* getSpanContext(PyObject* object) noexcept;

//--------------------------------------------------------------------------------------------------
// setupSpanContextClass
//--------------------------------------------------------------------------------------------------
bool setupSpanContextClass(PyObject* module) noexcept;
} // namespace python_bridge_tracer
