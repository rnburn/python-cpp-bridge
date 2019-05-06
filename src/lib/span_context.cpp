#include "span_context.h"

#include "python_bridge_tracer/module.h"

#include "span_context_bridge.h"
#include "utility.h"

static PyObject* SpanContextType;

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// SpanContextObject
//--------------------------------------------------------------------------------------------------
namespace {
struct SpanContextObject {
  PyObject_HEAD
  SpanContextBridge* span_context_bridge;
};
} // namespace

//--------------------------------------------------------------------------------------------------
// deallocSpanContext
//--------------------------------------------------------------------------------------------------
static void deallocSpanContext(SpanContextObject* self) noexcept {
  delete self->span_context_bridge;
}

//--------------------------------------------------------------------------------------------------
// makeSpanContext
//--------------------------------------------------------------------------------------------------
static PyObject* makeSpanContext(
    std::unique_ptr<SpanContextBridge>&& span_context_bridge) noexcept {
  auto result = PyObject_New(SpanContextObject,
                             reinterpret_cast<PyTypeObject*>(SpanContextType));
  if (result == nullptr) {
    return nullptr;
  }
  result->span_context_bridge = span_context_bridge.release();
  return reinterpret_cast<PyObject*>(result);
}

PyObject* makeSpanContext(
    std::unique_ptr<const opentracing::SpanContext>&& span_context) noexcept {
  return makeSpanContext(std::unique_ptr<SpanContextBridge>{
      new SpanContextBridge{std::move(span_context)}});
}

PyObject* makeSpanContext(std::shared_ptr<const opentracing::Span>& span) noexcept {
  return makeSpanContext(
      std::unique_ptr<SpanContextBridge>{new SpanContextBridge{span}});
}

//--------------------------------------------------------------------------------------------------
// getSpanContext
//--------------------------------------------------------------------------------------------------
const opentracing::SpanContext* getSpanContext(PyObject* object) noexcept {
  if (object->ob_type != reinterpret_cast<PyTypeObject*>(SpanContextType)) {
    PyErr_Format(PyExc_TypeError,
                 "expected " PYTHON_BRIDGE_TRACER_MODULE "._SpanContext");
    return nullptr;
  }
  return &reinterpret_cast<SpanContextObject*>(object)
              ->span_context_bridge->span_context();
}

//--------------------------------------------------------------------------------------------------
// SpanContextTypeSlots
//--------------------------------------------------------------------------------------------------
static PyType_Slot SpanContextTypeSlots[] = {
    {Py_tp_doc, toVoidPtr("CppBridgeSpanContext")},
    {Py_tp_dealloc, toVoidPtr(deallocSpanContext)},
    /* {Py_tp_getset, toVoidPtr(TracerGetSetList)}, */
    {0, nullptr}};

//--------------------------------------------------------------------------------------------------
// SpanContextTypeSpec
//--------------------------------------------------------------------------------------------------
static PyType_Spec SpanContextTypeSpec = {PYTHON_BRIDGE_TRACER_MODULE "._SpanContext",
                                     sizeof(SpanContextObject), 0,
                                     Py_TPFLAGS_DEFAULT, SpanContextTypeSlots};

//--------------------------------------------------------------------------------------------------
// setupSpanContextClass
//--------------------------------------------------------------------------------------------------
bool setupSpanContextClass(PyObject* module) noexcept {
  auto span_context_type = PyType_FromSpec(&SpanContextTypeSpec);
  if (span_context_type == nullptr) {
    return false;
  }
  SpanContextType = span_context_type;
  auto rcode = PyModule_AddObject(module, "_SpanContext", span_context_type);
  if (rcode != 0) {
    return false;
  }
  return true;
}
} // namespace python_bridge_tracer
