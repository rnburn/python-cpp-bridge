import bridge_tracer
import tempfile
import os
import json
import unittest

def make_mock_tracer(scope_manager = None):
    traces_path = os.path.join(tempfile.mkdtemp(prefix='python-bridge-test.'), 'traces.json')
    tracer = bridge_tracer.load_tracer(
            'external/io_opentracing_cpp/mocktracer/libmocktracer_plugin.so',
            '{ "output_file" : "%s" }' % traces_path)
    return tracer, traces_path

def read_spans(traces_path):
    with open(traces_path) as f:
        return json.loads(f.read())

class TestTracer(unittest.TestCase):
    def test_start_span(self):
        tracer, traces_path = make_mock_tracer()
        span = tracer.start_span('abc')
        span.finish()
        tracer.close()
        spans = read_spans(traces_path)
        self.assertEqual(len(spans), 1)
        self.assertEqual(spans[0]['operation_name'], 'abc')

    def test_get_tracer_from_span(self):
        tracer, traces_path = make_mock_tracer()
        span1 = tracer.start_span('abc')
        span2 = span1.tracer.start_span('123')
        span1.finish()
        span2.finish()
        tracer.close()
        spans = read_spans(traces_path)
        self.assertEqual(len(spans), 2)

    def test_context(self):
        tracer, traces_path = make_mock_tracer()
        span = tracer.start_span('abc')
        print(span.context)

    def test_start_active_span(self):
        tracer, traces_path = make_mock_tracer()
        print(tracer.active_span)
        scope = tracer.start_active_span('abc')
        print(scope)
        print(tracer.active_span)

    def test_properties(self):
        tracer, traces_path = make_mock_tracer()
        print(tracer.scope_manager)

if __name__ == '__main__':
    unittest.main()
