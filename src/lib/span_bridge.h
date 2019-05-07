#pragma once

#include <memory>

#include <Python.h>

#include "opentracing/span.h"

namespace python_bridge_tracer {
class SpanBridge {
 public:
   explicit SpanBridge(std::unique_ptr<opentracing::Span>&& span) noexcept;

   explicit SpanBridge(std::shared_ptr<opentracing::Span> span) noexcept;

   opentracing::Span& span() noexcept { return *span_; }

   PyObject* setTag(PyObject* args, PyObject* keywords) noexcept;

   PyObject* finish(PyObject* args, PyObject* keywords) noexcept;
 private:
  std::shared_ptr<opentracing::Span> span_;
};
} // namespace python_bridge_tracer
