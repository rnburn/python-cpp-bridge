import bridge_tracer

print(bridge_tracer.Tracer)
# print(bridge_tracer.loadTracer('abc', '123'))
print(bridge_tracer.loadTracer('external/io_opentracing_cpp/mocktracer/libmocktracer_plugin.so', 'test/mocktracer-config.json'))
