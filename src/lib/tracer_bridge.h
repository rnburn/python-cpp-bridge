#pragma once

#include <opentracing/tracer.h>

namespace python_bridge_tracer {
class TracerBridge {
 public:
   explicit TracerBridge(std::shared_ptr<opentracing::Tracer> tracer) noexcept;

   opentracing::Tracer& tracer() noexcept { return *tracer_; }
 private:
   std::shared_ptr<opentracing::Tracer> tracer_;
};
} // namespace python_bridge_tracer
