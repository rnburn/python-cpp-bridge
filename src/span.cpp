#include "span.h"

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
};
} // namespace

//--------------------------------------------------------------------------------------------------
// deallocSpan
//--------------------------------------------------------------------------------------------------
static void deallocSpan(SpanObject* self) noexcept {
  std::cerr << "yip" << std::endl;
  std::cerr << "span_bridge = " << self->span_bridge << std::endl;
  delete self->span_bridge;
  std::cerr << "nuf" << std::endl;
}

//--------------------------------------------------------------------------------------------------
// setTag
//--------------------------------------------------------------------------------------------------
static PyObject* setTag(SpanObject* self, PyObject* args, PyObject* keywords) noexcept {
  static char* keyword_names[] = {const_cast<char*>("key"),
                                  const_cast<char*>("value"),
                                  nullptr};
  const char* key;
  int key_length;
  PyObject* value;
  if (!PyArg_ParseTupleAndKeywords(args, keywords, "s#O:set_tag", keyword_names,
                                   &key, &key_length, &value)) {
    return nullptr;
  }
  opentracing::Value cpp_value;
  if (PyUnicode_Check(value) == 1) {
    char* s;
    auto rcode = PyBytes_AsStringAndSize(value, &s, nullptr);
    if (rcode == -1) {
      return nullptr;
    }
    cpp_value = s;
  } else if (PyBool_Check(value) == 1) {
    cpp_value = static_cast<bool>(PyObject_IsTrue(value));
  } else if (PyLong_Check(value) == 1) {
    auto long_value = PyLong_AsLong(value);
    if (long_value == -1 && PyErr_Occurred()) {
      return nullptr;
    }
    cpp_value = long_value;
  } else if (PyFloat_Check(value) == 1) {
    auto double_value = PyFloat_AsDouble(value);
    if (PyErr_Occurred()) {
      return nullptr;
    }
    cpp_value = double_value;
  } else {
    // TODO: error
    Py_RETURN_NONE;
  }
  self->span_bridge->span().SetTag(
      opentracing::string_view{key, static_cast<size_t>(key_length)},
      cpp_value);
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// finish
//--------------------------------------------------------------------------------------------------
static PyObject* finish(SpanObject* self, PyObject* args, PyObject* keywords) noexcept {
  self->span_bridge->span().Finish();
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// SpanMethods
//--------------------------------------------------------------------------------------------------
static PyMethodDef SpanMethods[] = {
    {"set_tag", reinterpret_cast<PyCFunction>(setTag),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("Set a tag")},
    {"finish", reinterpret_cast<PyCFunction>(finish),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("finish the span")},
    {nullptr, nullptr}};

//--------------------------------------------------------------------------------------------------
// SpanTypeSlots
//--------------------------------------------------------------------------------------------------
static PyType_Slot SpanTypeSlots[] = {
    {Py_tp_doc, toVoidPtr("CppBridgeSpan")},
    {Py_tp_dealloc, toVoidPtr(deallocSpan)},
    {Py_tp_methods, toVoidPtr(SpanMethods)},
    {0, nullptr}};

//--------------------------------------------------------------------------------------------------
// SpanTypeSpec
//--------------------------------------------------------------------------------------------------
static PyType_Spec SpanTypeSpec = {
    "bridge_tracer.Span", sizeof(SpanObject), 0,
    Py_TPFLAGS_DEFAULT,
    SpanTypeSlots};

//--------------------------------------------------------------------------------------------------
// startSpan
//--------------------------------------------------------------------------------------------------
PyObject* makeSpan(opentracing::Tracer& tracer,
                   opentracing::string_view operation_name, PyObject* parent,
                   PyObject* references, PyObject* tags,
                   double start_time) noexcept try {
  auto span = tracer.StartSpan(operation_name);
  auto result = PyObject_New(SpanObject, reinterpret_cast<PyTypeObject*>(SpanType));
  if (result == nullptr) {
    return nullptr;
  }
  result->span_bridge = new SpanBridge{std::shared_ptr<opentracing::Span>{span.release()}};
  return reinterpret_cast<PyObject*>(result);
} catch (const std::exception& e) {
  // TODO: error out
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// setupSpanClass
//--------------------------------------------------------------------------------------------------
void setupSpanClass(PyObject* module) noexcept {
  auto span_type = PyType_FromSpec(&SpanTypeSpec);
  if (span_type == nullptr) {
    // TODO: Fail?
    return;
  }
  SpanType = span_type;
  auto rcode = PyModule_AddObject(module, "Span", span_type);
  (void)rcode;
}
} // namespace python_bridge_tracer
