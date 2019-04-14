import bridge_tracer

print(bridge_tracer.Tracer)

tracer = bridge_tracer.loadTracer(
        'external/io_opentracing_cpp/mocktracer/libmocktracer_plugin.so', 
        '{ "output_file" : "/tmp/traces.json" }')
print(tracer)
span = tracer.start_span("abc")
print(span)
span.finish()
tracer.close()
