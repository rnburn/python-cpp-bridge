#pragma once

#include <opentracing/tracer.h>

namespace python_bridge_tracer {
std::shared_ptr<opentracing::Tracer> loadTracer(
    const char* tracer_library, const char* config);
}  // namespace python_bridge_tracer
