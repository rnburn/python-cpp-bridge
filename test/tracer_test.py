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

if __name__ == '__main__':
    unittest.main()
