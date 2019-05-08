import bridge_tracer
import tempfile
import os
import json
import unittest
import opentracing

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

    def test_start_span_with_reference1(self):
        tracer, traces_path = make_mock_tracer()
        spanA = tracer.start_span('A')
        spanB = tracer.start_span('B', child_of=spanA)
        spanB.finish()
        spanA.finish()
        tracer.close()
        spans = read_spans(traces_path)
        self.assertEqual(len(spans), 2)
        traceIdA = spans[1]['span_context']['trace_id']
        spanIdA = spans[1]['span_context']['span_id']
        traceIdB = spans[0]['span_context']['trace_id']
        self.assertEqual(traceIdA, traceIdB)
        references = spans[0]['references']
        self.assertEqual(len(references), 1)
        self.assertEqual(references[0]['reference_type'], 'CHILD_OF')
        self.assertEqual(references[0]['trace_id'], traceIdA)
        self.assertEqual(references[0]['span_id'], spanIdA)

    def test_start_span_with_reference2(self):
        tracer, traces_path = make_mock_tracer()
        spanA = tracer.start_span('A')
        spanB = tracer.start_span('B', child_of=spanA.context)
        spanB.finish()
        spanA.finish()
        tracer.close()
        spans = read_spans(traces_path)
        self.assertEqual(len(spans), 2)
        traceIdA = spans[1]['span_context']['trace_id']
        spanIdA = spans[1]['span_context']['span_id']
        traceIdB = spans[0]['span_context']['trace_id']
        self.assertEqual(traceIdA, traceIdB)
        references = spans[0]['references']
        self.assertEqual(len(references), 1)
        self.assertEqual(references[0]['reference_type'], 'CHILD_OF')
        self.assertEqual(references[0]['trace_id'], traceIdA)
        self.assertEqual(references[0]['span_id'], spanIdA)

    def test_start_span_with_reference3(self):
        tracer, traces_path = make_mock_tracer()
        spanA = tracer.start_span('A')
        spanB = tracer.start_span('B', references=[
            opentracing.Reference(opentracing.ReferenceType.CHILD_OF, spanA.context)])
        spanB.finish()
        spanA.finish()
        tracer.close()
        spans = read_spans(traces_path)
        self.assertEqual(len(spans), 2)
        traceIdA = spans[1]['span_context']['trace_id']
        spanIdA = spans[1]['span_context']['span_id']
        traceIdB = spans[0]['span_context']['trace_id']
        self.assertEqual(traceIdA, traceIdB)
        references = spans[0]['references']
        self.assertEqual(len(references), 1)
        self.assertEqual(references[0]['reference_type'], 'CHILD_OF')
        self.assertEqual(references[0]['trace_id'], traceIdA)
        self.assertEqual(references[0]['span_id'], spanIdA)

    def test_start_span_with_reference4(self):
        tracer, traces_path = make_mock_tracer()
        spanA = tracer.start_active_span('A').span
        spanB = tracer.start_span('B')
        spanB.finish()
        spanA.finish()
        tracer.close()
        spans = read_spans(traces_path)
        self.assertEqual(len(spans), 2)
        traceIdA = spans[1]['span_context']['trace_id']
        spanIdA = spans[1]['span_context']['span_id']
        traceIdB = spans[0]['span_context']['trace_id']
        self.assertEqual(traceIdA, traceIdB)
        references = spans[0]['references']
        self.assertEqual(len(references), 1)
        self.assertEqual(references[0]['reference_type'], 'CHILD_OF')
        self.assertEqual(references[0]['trace_id'], traceIdA)
        self.assertEqual(references[0]['span_id'], spanIdA)

    def test_set_operation_name(self):
        tracer, traces_path = make_mock_tracer()
        span = tracer.start_span('abc')
        span.set_operation_name('xyz')
        span.finish()
        tracer.close()
        spans = read_spans(traces_path)
        self.assertEqual(len(spans), 1)
        self.assertEqual(spans[0]['operation_name'], 'xyz')

    def test_set_tag1(self):
        tracer, traces_path = make_mock_tracer()
        span = tracer.start_span('abc')
        span.set_tag('a', 1)
        span.finish()
        tracer.close()
        spans = read_spans(traces_path)
        self.assertEqual(len(spans), 1)
        self.assertEqual(spans[0]['tags']['a'], 1)

    def test_set_tag2(self):
        tracer, traces_path = make_mock_tracer()
        span = tracer.start_span('abc', tags={'a':1})
        span.finish()
        tracer.close()
        spans = read_spans(traces_path)
        self.assertEqual(len(spans), 1)
        self.assertEqual(spans[0]['tags']['a'], 1)


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
