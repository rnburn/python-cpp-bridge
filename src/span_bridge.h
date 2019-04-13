#pragma once

#include <memory>

#include <opentracing/span.h>

namespace python_bridge_tracer {
class SpanBridge {
 public:
   explicit SpanBridge(std::shared_ptr<opentracing::Span> span) noexcept;
 private:
  std::shared_ptr<opentracing::Span> span_;
};
} // namespace python_bridge_tracer
