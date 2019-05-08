#pragma once

#include <memory>

#include <Python.h>

#include "opentracing/span.h"

namespace python_bridge_tracer {
class SpanBridge {
 public:
   explicit SpanBridge(std::unique_ptr<opentracing::Span>&& span) noexcept;

   explicit SpanBridge(std::shared_ptr<opentracing::Span> span) noexcept;

   std::shared_ptr<const opentracing::Span> span() noexcept { return span_; }

   PyObject* setOperationName(PyObject* args, PyObject* keywords) noexcept;

   PyObject* setTag(PyObject* args, PyObject* keywords) noexcept;

   PyObject* finish(PyObject* args, PyObject* keywords) noexcept;

   PyObject* exit(PyObject* args) noexcept;
 private:
  std::shared_ptr<opentracing::Span> span_;
  opentracing::FinishSpanOptions finish_span_options_;
};
} // namespace python_bridge_tracer
