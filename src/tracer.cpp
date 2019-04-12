#include "tracer.h"

#include <iostream>

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// TracerObject
//--------------------------------------------------------------------------------------------------
namespace {
struct TracerObject {
  PyObject_HEAD
  PyObject* attributes_dictionary;
};
} // namespace

//--------------------------------------------------------------------------------------------------
// traverseTracer
//--------------------------------------------------------------------------------------------------
static int traverseTracer(TracerObject* self, visitproc visit, void* arg) noexcept {
  Py_VISIT(self->attributes_dictionary);
  return 0;
}

//--------------------------------------------------------------------------------------------------
// getattr
//--------------------------------------------------------------------------------------------------
static PyObject* getattr(TracerObject* self, PyObject* name) noexcept {
  if (self->attributes_dictionary != nullptr) {
    PyObject* v = PyDict_GetItemWithError(self->attributes_dictionary, name);
    if (v != nullptr) {
      Py_INCREF(v);
      return v;
    } else if (PyErr_Occurred()) {
      return nullptr;
    }
  }
  return PyObject_GenericGetAttr(reinterpret_cast<PyObject*>(self), name);
}

//--------------------------------------------------------------------------------------------------
// setattr
//--------------------------------------------------------------------------------------------------
static int setattr(TracerObject* self, const char* name, PyObject* value) noexcept {
    if (self->attributes_dictionary == nullptr) {
        self->attributes_dictionary = PyDict_New();
        if (self->attributes_dictionary == nullptr) {
            return -1;
        }
    }
    if (value == nullptr) {
        int result = PyDict_DelItemString(self->attributes_dictionary, name);
        if (result < 0 && PyErr_ExceptionMatches(PyExc_KeyError)) {
            PyErr_SetString(PyExc_AttributeError,
                "delete non-existing Example attribute");
        }
        return result;
    }
    return PyDict_SetItemString(self->attributes_dictionary, name, value);
}

//--------------------------------------------------------------------------------------------------
// deallocTracer
//--------------------------------------------------------------------------------------------------
static void deallocTracer(TracerObject* self) noexcept {
  Py_CLEAR(self->attributes_dictionary);
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
    {Py_tp_traverse, toVoidPtr(traverseTracer)},
    {Py_tp_getattro, toVoidPtr(getattr)},
    {Py_tp_setattr, toVoidPtr(setattr)},
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
