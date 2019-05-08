#include "span.h"

#include "python_bridge_tracer/module.h"

#include "span_bridge.h"
#include "span_context.h"
#include "utility.h"

#include <iostream>

static PyObject* SpanType;

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// SpanObject
//--------------------------------------------------------------------------------------------------
namespace {
struct SpanObject {
  PyObject_HEAD
  SpanBridge* span_bridge;
  PyObject* tracer;
};
} // namespace

//--------------------------------------------------------------------------------------------------
// deallocSpan
//--------------------------------------------------------------------------------------------------
static void deallocSpan(SpanObject* self) noexcept {
  delete self->span_bridge;
  Py_DECREF(self->tracer);
}

//--------------------------------------------------------------------------------------------------
// setOperationName
//--------------------------------------------------------------------------------------------------
static PyObject* setOperationName(SpanObject* self, PyObject* args, PyObject* keywords) noexcept {
  // TODO: fill in
  (void)self;
  (void)args;
  (void)keywords;
  Py_RETURN_NONE;
}


//--------------------------------------------------------------------------------------------------
// setTag
//--------------------------------------------------------------------------------------------------
static PyObject* setTag(SpanObject* self, PyObject* args, PyObject* keywords) noexcept {
  return self->span_bridge->setTag(args, keywords);
}

//--------------------------------------------------------------------------------------------------
// logKeyValues
//--------------------------------------------------------------------------------------------------
static PyObject* logKeyValues(SpanObject* self, PyObject* args, PyObject* keywords) noexcept {
  // TODO: fill in
  (void)self;
  (void)args;
  (void)keywords;
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// setBaggageItem
//--------------------------------------------------------------------------------------------------
static PyObject* setBaggageItem(SpanObject* self, PyObject* args, PyObject* keywords) noexcept {
  // TODO: fill in
  (void)self;
  (void)args;
  (void)keywords;
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// getBaggageItem
//--------------------------------------------------------------------------------------------------
static PyObject* getBaggageItem(SpanObject* self, PyObject* args, PyObject* keywords) noexcept {
  // TODO: fill in
  (void)self;
  (void)args;
  (void)keywords;
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// logEvent
//--------------------------------------------------------------------------------------------------
static PyObject* logEvent(SpanObject* self, PyObject* args, PyObject* keywords) noexcept {
  // TODO: fill in
  (void)self;
  (void)args;
  (void)keywords;
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// log
//--------------------------------------------------------------------------------------------------
static PyObject* log(SpanObject* self, PyObject* args, PyObject* keywords) noexcept {
  // TODO: fill in
  (void)self;
  (void)args;
  (void)keywords;
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// finish
//--------------------------------------------------------------------------------------------------
static PyObject* finish(SpanObject* self, PyObject* args, PyObject* keywords) noexcept {
  return self->span_bridge->finish(args, keywords);
}

//--------------------------------------------------------------------------------------------------
// enterContext
//--------------------------------------------------------------------------------------------------
static PyObject* enterContext(PyObject* self, PyObject* /*args*/) noexcept {
  Py_INCREF(self);
  return self;
}

//--------------------------------------------------------------------------------------------------
// exitContext
//--------------------------------------------------------------------------------------------------
static PyObject* exitContext(SpanObject* self, PyObject* args) noexcept {
  return self->span_bridge->exit(args);
}

//--------------------------------------------------------------------------------------------------
// getContext
//--------------------------------------------------------------------------------------------------
static PyObject* getContext(SpanObject* self, PyObject* /*ignored*/) noexcept {
  return makeSpanContext(std::unique_ptr<SpanContextBridge>{
      new SpanContextBridge{self->span_bridge->span()}});
}

//--------------------------------------------------------------------------------------------------
// getTracer
//--------------------------------------------------------------------------------------------------
static PyObject* getTracer(SpanObject* self, PyObject* /*ignored*/) noexcept {
  Py_INCREF(self->tracer);
  return self->tracer;
}

//--------------------------------------------------------------------------------------------------
// SpanMethods
//--------------------------------------------------------------------------------------------------
static PyMethodDef SpanMethods[] = {
    {"set_operation_name", reinterpret_cast<PyCFunction>(setOperationName),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("set the span's operation name")},
    {"set_tag", reinterpret_cast<PyCFunction>(setTag),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("set a tag")},
    {"log_kv", reinterpret_cast<PyCFunction>(logKeyValues),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("log key-values")},
    {"set_baggage_item", reinterpret_cast<PyCFunction>(setBaggageItem),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("stores a baggage item")},
    {"get_baggage_item", reinterpret_cast<PyCFunction>(getBaggageItem),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("retrieves a baggage item")},
    {"log_event", reinterpret_cast<PyCFunction>(logEvent),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("log an event")},
    {"log", reinterpret_cast<PyCFunction>(log),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("log key-values")},
    {"finish", reinterpret_cast<PyCFunction>(finish),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("finish the span")},
    {"__enter__", reinterpret_cast<PyCFunction>(enterContext), METH_NOARGS, nullptr},
    {"__exit__", reinterpret_cast<PyCFunction>(exitContext), METH_VARARGS, nullptr},
    {nullptr, nullptr}};

//--------------------------------------------------------------------------------------------------
// SpanGetSetList
//--------------------------------------------------------------------------------------------------
static PyGetSetDef SpanGetSetList[] = {
    {"context", reinterpret_cast<getter>(getContext), nullptr,
     PyDoc_STR("Returns the span's context")},
    {"tracer", reinterpret_cast<getter>(getTracer), nullptr,
     PyDoc_STR("Returns the tracer used to create the span")},
    {nullptr}};

//--------------------------------------------------------------------------------------------------
// SpanTypeSlots
//--------------------------------------------------------------------------------------------------
static PyType_Slot SpanTypeSlots[] = {
    {Py_tp_doc, toVoidPtr("CppBridgeSpan")},
    {Py_tp_dealloc, toVoidPtr(deallocSpan)},
    {Py_tp_methods, toVoidPtr(SpanMethods)},
    {Py_tp_getset, toVoidPtr(SpanGetSetList)},
    {0, nullptr}};

//--------------------------------------------------------------------------------------------------
// SpanTypeSpec
//--------------------------------------------------------------------------------------------------
static PyType_Spec SpanTypeSpec = {PYTHON_BRIDGE_TRACER_MODULE "._Span",
                                   sizeof(SpanObject), 0, Py_TPFLAGS_DEFAULT,
                                   SpanTypeSlots};

//--------------------------------------------------------------------------------------------------
// startSpan
//--------------------------------------------------------------------------------------------------
PyObject* makeSpan(std::unique_ptr<SpanBridge>&& span_bridge, PyObject* tracer) noexcept {
  auto result = PyObject_New(SpanObject, reinterpret_cast<PyTypeObject*>(SpanType));
  if (result == nullptr) {
    return nullptr;
  }
  result->span_bridge = span_bridge.release();
  Py_INCREF(tracer);
  result->tracer = tracer;
  return reinterpret_cast<PyObject*>(result);
}

//--------------------------------------------------------------------------------------------------
// isSpan
//--------------------------------------------------------------------------------------------------
bool isSpan(PyObject* object) noexcept {
  return object->ob_type == reinterpret_cast<PyTypeObject*>(SpanType);
}

//--------------------------------------------------------------------------------------------------
// getSpanContext
//--------------------------------------------------------------------------------------------------
SpanContextBridge getSpanContextFromSpan(PyObject* object) noexcept {
  assert(isSpan(object));
  return SpanContextBridge{reinterpret_cast<SpanObject*>(object)->span_bridge->span()};
}

//--------------------------------------------------------------------------------------------------
// setupSpanClass
//--------------------------------------------------------------------------------------------------
bool setupSpanClass(PyObject* module) noexcept {
  auto span_type = PyType_FromSpec(&SpanTypeSpec);
  if (span_type == nullptr) {
    return false;
  }
  SpanType = span_type;
  auto rcode = PyModule_AddObject(module, "_Span", span_type);
  if (rcode != 0) {
    return false;
  }
  return true;
}
} // namespace python_bridge_tracer
