#pragma once

#include <Python.h>

#include "span_bridge.h"

#include "opentracing/tracer.h"

namespace python_bridge_tracer {
class TracerBridge {
 public:
   explicit TracerBridge(std::shared_ptr<opentracing::Tracer> tracer) noexcept;

   opentracing::Tracer& tracer() noexcept { return *tracer_; }

   std::unique_ptr<SpanBridge> makeSpan(opentracing::string_view operation_name,
                                        PyObject* scope_manager,
                                        PyObject* parent, PyObject* references,
                                        PyObject* tags, double start_time,
                                        bool ignore_active_span) noexcept;

  private:
   std::shared_ptr<opentracing::Tracer> tracer_;
};
} // namespace python_bridge_tracer
