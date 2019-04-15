#pragma once

#include <memory>

#include <opentracing/span.h>

namespace python_bridge_tracer {
class SpanBridge {
 public:
   explicit SpanBridge(std::shared_ptr<opentracing::Span> span) noexcept;

   opentracing::Span& span() noexcept { return *span_; }
 private:
  std::shared_ptr<opentracing::Span> span_;
};
} // namespace python_bridge_tracer
