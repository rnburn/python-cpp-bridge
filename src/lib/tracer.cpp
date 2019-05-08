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
    const_cast<char*>("finish_on_close"),
    nullptr
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
  auto span_bridge = self->tracer_bridge->makeSpan(
      opentracing::string_view{operation_name,
                               static_cast<size_t>(operation_name_length)},
      self->scope_manager, parent, references, tags, start_time,
      static_cast<bool>(ignore_active_span));
  auto span =
      makeSpan(std::move(span_bridge), reinterpret_cast<PyObject*>(self));
  if (span == nullptr) {
    return nullptr;
  }
  auto cleanup_span = finally([span] { Py_DECREF(span); });
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
  auto span_bridge = self->tracer_bridge->makeSpan(
      opentracing::string_view{operation_name,
                               static_cast<size_t>(operation_name_length)},
      self->scope_manager, parent, references, tags, start_time,
      static_cast<bool>(ignore_active_span));
  return makeSpan(std::move(span_bridge), reinterpret_cast<PyObject*>(self));
}

//--------------------------------------------------------------------------------------------------
// inject
//--------------------------------------------------------------------------------------------------
static PyObject* inject(TracerObject* self, PyObject* args, PyObject* keywords) noexcept {
  // TODO: fill in
  (void)self;
  (void)args;
  (void)keywords;
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// extract
//--------------------------------------------------------------------------------------------------
static PyObject* extract(TracerObject* self, PyObject* args, PyObject* keywords) noexcept {
  // TODO: fill in
  (void)self;
  (void)args;
  (void)keywords;
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// close
//--------------------------------------------------------------------------------------------------
static PyObject* close(TracerObject* self) noexcept {
  self->tracer_bridge->tracer().Close();
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// getScopeManager
//--------------------------------------------------------------------------------------------------
static PyObject* getScopeManager(TracerObject* self, void* /*ignored*/) noexcept {
  Py_INCREF(self->scope_manager);
  return self->scope_manager;
}

//--------------------------------------------------------------------------------------------------
// getActiveSpan
//--------------------------------------------------------------------------------------------------
static PyObject* getActiveSpan(TracerObject* self, void* /*ignored*/) noexcept {
  auto scope = PyObject_GetAttrString(self->scope_manager, "active");
  if (scope == nullptr) {
    return nullptr;
  }
  auto cleanup_scope = finally([scope] {
      Py_DECREF(scope);
  });
  if (scope == Py_None) {
    Py_RETURN_NONE;
  }
  return PyObject_GetAttrString(scope, "span");
}

//--------------------------------------------------------------------------------------------------
// TracerMethods
//--------------------------------------------------------------------------------------------------
static PyMethodDef TracerMethods[] = {
    {"start_span", reinterpret_cast<PyCFunction>(startSpan),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("start a span")},
    {"start_active_span", reinterpret_cast<PyCFunction>(startActiveSpan),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("start and activate a span")},
    {"inject", reinterpret_cast<PyCFunction>(inject),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("injects a span's context into a carrier")},
    {"extract", reinterpret_cast<PyCFunction>(extract),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("extracts a span's context from a carrier")},
    {"close", reinterpret_cast<PyCFunction>(close), METH_VARARGS,
     PyDoc_STR("close tracer")},
    {nullptr, nullptr}};

//--------------------------------------------------------------------------------------------------
// TracerGetSetList
//--------------------------------------------------------------------------------------------------
static PyGetSetDef TracerGetSetList[] = {
    {"scope_manager", reinterpret_cast<getter>(getScopeManager), nullptr,
     PyDoc_STR("Returns the attached ScopeManager")},
    {"active_span", reinterpret_cast<getter>(getActiveSpan), nullptr,
     PyDoc_STR("Returns the active span")},
    {nullptr}};

//--------------------------------------------------------------------------------------------------
// TracerTypeSlots
//--------------------------------------------------------------------------------------------------
static PyType_Slot TracerTypeSlots[] = {
    {Py_tp_doc, toVoidPtr("CppBridgeTracer")},
    {Py_tp_dealloc, toVoidPtr(deallocTracer)},
    {Py_tp_methods, toVoidPtr(TracerMethods)},
    {Py_tp_getset, toVoidPtr(TracerGetSetList)},
    {0, nullptr}};

//--------------------------------------------------------------------------------------------------
// TracerTypeSpec
//--------------------------------------------------------------------------------------------------
static PyType_Spec TracerTypeSpec = {PYTHON_BRIDGE_TRACER_MODULE "._Tracer",
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
  auto rcode = PyModule_AddObject(module, "_Tracer", tracer_type);
  if (rcode != 0) {
    return false;
  }
  return true;
}
} // namespace python_bridge_tracer
