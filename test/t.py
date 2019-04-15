import bridge_tracer

print(bridge_tracer.Tracer)

tracer = bridge_tracer.load_tracer(
        'external/io_opentracing_cpp/mocktracer/libmocktracer_plugin.so', 
        '{ "output_file" : "/tmp/traces.json" }')
print(tracer)
span = tracer.start_span("abc")
print(span)
span.set_tag('abc', '123')
span.finish()
tracer.close()
