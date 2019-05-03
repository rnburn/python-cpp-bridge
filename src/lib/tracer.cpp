#include "tracer.h"

#include <iostream>
#include <memory>

#include "python_bridge_tracer/module.h"

#include "tracer_bridge.h"
#include "span.h"
#include "utility.h"

static PyObject* TracerType;

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// TracerObject
//--------------------------------------------------------------------------------------------------
namespace {
struct TracerObject {
  PyObject_HEAD
  TracerBridge* tracer_bridge;
  PyObject* scope_manager;
};
} // namespace

//--------------------------------------------------------------------------------------------------
// deallocTracer
//--------------------------------------------------------------------------------------------------
static void deallocTracer(TracerObject* self) noexcept {
  delete self->tracer_bridge;
  Py_DECREF(self->scope_manager);
}

//--------------------------------------------------------------------------------------------------
// activateSpan
//--------------------------------------------------------------------------------------------------
static PyObject* activateSpan(TracerObject* self, PyObject* span, int finish_on_close) noexcept {
  auto args = Py_BuildValue("Oi", span, finish_on_close);
  if (args == nullptr) {
    return nullptr;
  }
  auto cleanup_args = finally([args] {
      Py_DECREF(args);
  });
  auto activate_function = PyObject_GetAttrString(self->scope_manager, "activate");
  if (activate_function == nullptr) {
    return nullptr;
  }
  auto cleanup_activate_function = finally([activate_function] {
      Py_DECREF(activate_function);
  });
  return PyObject_CallObject(activate_function, args);
}

//--------------------------------------------------------------------------------------------------
// startActiveSpan
//--------------------------------------------------------------------------------------------------
static PyObject* startActiveSpan(TracerObject* self, PyObject* args, PyObject* keywords) noexcept {
  static char* keyword_names[] = {
    const_cast<char*>("operation_name"),
    const_cast<char*>("child_of"),
    const_cast<char*>("references"),
    const_cast<char*>("tags"),
    const_cast<char*>("start_time"),
    const_cast<char*>("ignore_active_span"),
    const_cast<char*>("finish_on_close")
  };
  const char* operation_name = nullptr;
  int operation_name_length = 0;
  PyObject* parent = nullptr;
  PyObject* references = nullptr;
  PyObject* tags = nullptr;
  double start_time = 0;
  int ignore_active_span = 0;
  int finish_on_close = 1;
  static const char* const arguments_format =
      "s#"  // operation_name
      "|"
      "O"  // parent
      "O"  // references
      "O"  // tags
      "d"  // start_time
      "p"  // ignore_active_span
      "p" // finish_on_close
      ":start_active_span";
  if (!PyArg_ParseTupleAndKeywords(
          args, keywords, arguments_format, keyword_names, &operation_name,
          &operation_name_length, &parent, &references, &tags, &start_time,
          &ignore_active_span, &finish_on_close)) {
    return nullptr;
  }
  auto span =
      makeSpan(self->tracer_bridge->tracer(),
               opentracing::string_view{
                   operation_name, static_cast<size_t>(operation_name_length)},
               parent, references, tags, start_time);
  if (span == nullptr) {
    return nullptr;
  }
  return activateSpan(self, span, finish_on_close);
}

//--------------------------------------------------------------------------------------------------
// startSpan
//--------------------------------------------------------------------------------------------------
static PyObject* startSpan(TracerObject* self, PyObject* args, PyObject* keywords) noexcept {
  static char* keyword_names[] = {const_cast<char*>("operation_name"),
                                  const_cast<char*>("child_of"),
                                  const_cast<char*>("references"),
                                  const_cast<char*>("tags"),
                                  const_cast<char*>("start_time"),
                                  const_cast<char*>("ignore_active_span"), nullptr};
  const char* operation_name = nullptr;
  int operation_name_length = 0;
  PyObject* parent = nullptr;
  PyObject* references = nullptr;
  PyObject* tags = nullptr;
  double start_time = 0;
  int ignore_active_span = 0;
  static const char* const arguments_format =
      "s#"  // operation_name
      "|"
      "O"  // parent
      "O"  // references
      "O"  // tags
      "d"  // start_time
      "p"  // ignore_active_span
      ":start_span";
  if (!PyArg_ParseTupleAndKeywords(args, keywords, arguments_format,
                                   keyword_names, &operation_name,
                                   &operation_name_length, &parent, &references,
                                   &tags, &start_time, &ignore_active_span)) {
    return nullptr;
  }
  return makeSpan(
      self->tracer_bridge->tracer(),
      opentracing::string_view{operation_name,
                               static_cast<size_t>(operation_name_length)},
      parent, references, tags, start_time);
}

//--------------------------------------------------------------------------------------------------
// close
//--------------------------------------------------------------------------------------------------
static PyObject* close(TracerObject* self) {
  self->tracer_bridge->tracer().Close();
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// TracerMethods
//--------------------------------------------------------------------------------------------------
static PyMethodDef TracerMethods[] = {
    {"start_span", reinterpret_cast<PyCFunction>(startSpan),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("start a span")},
    {"start_active_span", reinterpret_cast<PyCFunction>(startActiveSpan),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("start and activate a span")},
    {"close", reinterpret_cast<PyCFunction>(close), METH_VARARGS,
     PyDoc_STR("close tracer")},
    {nullptr, nullptr}};

//--------------------------------------------------------------------------------------------------
// TracerTypeSlots
//--------------------------------------------------------------------------------------------------
static PyType_Slot TracerTypeSlots[] = {
    {Py_tp_doc, toVoidPtr("CppBridgeTracer")},
    {Py_tp_dealloc, toVoidPtr(deallocTracer)},
    {Py_tp_methods, toVoidPtr(TracerMethods)},
    {0, nullptr}};

//--------------------------------------------------------------------------------------------------
// TracerTypeSpec
//--------------------------------------------------------------------------------------------------
static PyType_Spec TracerTypeSpec = {PYTHON_BRIDGE_TRACER_MODULE ".Tracer",
                                     sizeof(TracerObject), 0,
                                     Py_TPFLAGS_DEFAULT, TracerTypeSlots};

//--------------------------------------------------------------------------------------------------
// makeTracer
//--------------------------------------------------------------------------------------------------
PyObject* makeTracer(std::shared_ptr<opentracing::Tracer> tracer,
                     PyObject* scope_manager) noexcept try {
  std::unique_ptr<TracerBridge> tracer_bridge{
      new TracerBridge{std::move(tracer)}};
  auto result =
      PyObject_New(TracerObject, reinterpret_cast<PyTypeObject*>(TracerType));
  if (result == nullptr) {
    return nullptr;
  }
  result->tracer_bridge = tracer_bridge.release();
  result->scope_manager = scope_manager;
  return reinterpret_cast<PyObject*>(result);
} catch (const std::exception& e) {
  PyErr_Format(PyExc_RuntimeError, e.what());
  return nullptr;
}

//--------------------------------------------------------------------------------------------------
// setupTracerClass
//--------------------------------------------------------------------------------------------------
bool setupTracerClass(PyObject* module) noexcept {
  auto tracer_type = PyType_FromSpec(&TracerTypeSpec);
  if (tracer_type == nullptr) {
    return false;
  }
  TracerType = tracer_type;
  auto rcode = PyModule_AddObject(module, "Tracer", tracer_type);
  if (rcode != 0) {
    return false;
  }
  return true;
}
} // namespace python_bridge_tracer
