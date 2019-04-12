#include "tracer.h"

#include <iostream>

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// TracerObject
//--------------------------------------------------------------------------------------------------
namespace {
struct TracerObject {
  PyObject_HEAD
};
} // namespace

//--------------------------------------------------------------------------------------------------
// deallocTracer
//--------------------------------------------------------------------------------------------------
static void deallocTracer(TracerObject* self) noexcept {
}

//--------------------------------------------------------------------------------------------------
// startSpan
//--------------------------------------------------------------------------------------------------
static PyObject* startSpan(TracerObject* self, PyObject* args) noexcept {
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// TracerMethods
//--------------------------------------------------------------------------------------------------
static PyMethodDef TracerMethods[] = {
    {"startSpan", reinterpret_cast<PyCFunction>(startSpan), METH_VARARGS,
     PyDoc_STR("opentracing-compliant tracer")},
    {nullptr, nullptr}};

//--------------------------------------------------------------------------------------------------
// toVoidPtr
//--------------------------------------------------------------------------------------------------
template <class T>
static void* toVoidPtr(T* t) noexcept {
  return reinterpret_cast<void*>(t);
}

template <class T>
static void* toVoidPtr(const T* t) noexcept {
  return toVoidPtr(const_cast<T*>(t));
}

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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_HAVE_FINALIZE,
    TracerTypeSlots};

//--------------------------------------------------------------------------------------------------
// setupTracerClass
//--------------------------------------------------------------------------------------------------
void setupTracerClass(PyObject* module) noexcept {
  auto tracer = PyType_FromSpec(&TracerTypeSpec);
  if (tracer == nullptr) {
    // TODO: Fail?
    return;
  }
  auto rcode = PyModule_AddObject(module, "Tracer", tracer);
  (void)rcode;
}
} // namespace python_bridge_tracer
