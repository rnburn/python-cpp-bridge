#include "span.h"

#include "python_bridge_tracer/module.h"

#include "span_bridge.h"
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
// setTag
//--------------------------------------------------------------------------------------------------
static PyObject* setTag(SpanObject* self, PyObject* args, PyObject* keywords) noexcept {
  return self->span_bridge->setTag(args, keywords);
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
  (void)args;
  self->span_bridge->span().Finish();
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// getContext
//--------------------------------------------------------------------------------------------------
static PyObject* getContext(SpanObject* self, PyObject* /*ignored*/) noexcept {
  (void)self;
  return nullptr;
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
    {"set_tag", reinterpret_cast<PyCFunction>(setTag),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("Set a tag")},
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
static PyType_Spec SpanTypeSpec = {PYTHON_BRIDGE_TRACER_MODULE ".Span",
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
// setupSpanClass
//--------------------------------------------------------------------------------------------------
bool setupSpanClass(PyObject* module) noexcept {
  auto span_type = PyType_FromSpec(&SpanTypeSpec);
  if (span_type == nullptr) {
    return false;
  }
  SpanType = span_type;
  auto rcode = PyModule_AddObject(module, "Span", span_type);
  if (rcode != 0) {
    return false;
  }
  return true;
}
} // namespace python_bridge_tracer
