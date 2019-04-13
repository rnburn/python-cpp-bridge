#include "tracer.h"

#include <iostream>

#include "dynamic_tracer.h"
#include "tracer_bridge.h"
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
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// TracerMethods
//--------------------------------------------------------------------------------------------------
static PyMethodDef TracerMethods[] = {
    {"startSpan", reinterpret_cast<PyCFunction>(startSpan), METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("opentracing-compliant tracer")},
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
} catch(const std::exception& /*e*/) {
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
