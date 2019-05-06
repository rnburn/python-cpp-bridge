#pragma once

#include "opentracing/span.h"

#include <memory>

namespace python_bridge_tracer {
class SpanContextBridge {
 public:
   explicit SpanContextBridge(const std::shared_ptr<const opentracing::Span>& span) noexcept;

   explicit SpanContextBridge(
       std::unique_ptr<const opentracing::SpanContext>&& span_context) noexcept;

   const opentracing::SpanContext& span_context() const noexcept;

 private:
   std::shared_ptr<const opentracing::Span> span_;
   std::unique_ptr<const opentracing::SpanContext> span_context_;
};
} // namespace python_bridge_tracer
