#include "tracer.h"

#include <iostream>

#include "dynamic_tracer.h"
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
};
} // namespace

//--------------------------------------------------------------------------------------------------
// deallocTracer
//--------------------------------------------------------------------------------------------------
static void deallocTracer(TracerObject* self) noexcept {
  delete self->tracer_bridge;
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
      ":startSpan";
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
static PyType_Spec TracerTypeSpec = {
    "bridge_tracer.Tracer", sizeof(TracerObject), 0,
    Py_TPFLAGS_DEFAULT,
    TracerTypeSlots};

//--------------------------------------------------------------------------------------------------
// loadTracer
//--------------------------------------------------------------------------------------------------
PyObject* loadTracer(PyObject* /*self*/, PyObject* args, PyObject* keywords) noexcept try {
  static char* keyword_names[] = {const_cast<char*>("library"),
                                  const_cast<char*>("config"),
                                  const_cast<char*>("scope_manager"), nullptr};
  char* library;
  char* config;
  PyObject* scope_manager = nullptr;
  if (!PyArg_ParseTupleAndKeywords(args, keywords, "ss|O:loadTracer", keyword_names, 
        &library, &config, &scope_manager)) {
    return nullptr;
  }
  auto tracer = loadTracer(library, config);
  auto result = PyObject_New(TracerObject, reinterpret_cast<PyTypeObject*>(TracerType));
  if (result == nullptr) {
    return nullptr;
  }
  result->tracer_bridge = new TracerBridge{std::move(tracer)};
  return reinterpret_cast<PyObject*>(result);
} catch(const std::exception& e) {
  std::cerr << "failed to load tracer: " << e.what() << "\n";
  // TODO: make exception
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// setupTracerClass
//--------------------------------------------------------------------------------------------------
void setupTracerClass(PyObject* module) noexcept {
  auto tracer_type = PyType_FromSpec(&TracerTypeSpec);
  if (tracer_type == nullptr) {
    // TODO: Fail?
    return;
  }
  TracerType = tracer_type;
  auto rcode = PyModule_AddObject(module, "Tracer", tracer_type);
  (void)rcode;
}
} // namespace python_bridge_tracer
