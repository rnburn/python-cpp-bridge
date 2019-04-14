#include <Python.h>

#include "tracer.h"
#include "span.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// ModuleMethods
//--------------------------------------------------------------------------------------------------
static PyMethodDef ModuleMethods[] = {
    {"loadTracer", reinterpret_cast<PyCFunction>(loadTracer),
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR("loads a C++ opentracing plugin")},
    {nullptr, nullptr}};

//--------------------------------------------------------------------------------------------------
// ModuleDefinition
//--------------------------------------------------------------------------------------------------
static PyModuleDef ModuleDefinition = {PyModuleDef_HEAD_INIT, "bridge_tracer",
                                       "doc string", -1, ModuleMethods};
} // namespace python_bridge_tracer

//--------------------------------------------------------------------------------------------------
// PyInit_bridge_tracer
//--------------------------------------------------------------------------------------------------
extern "C" {
PyMODINIT_FUNC PyInit_bridge_tracer() noexcept {
  using namespace python_bridge_tracer;
  auto module = PyModule_Create(&ModuleDefinition);
  if (module == nullptr) {
    return nullptr;
  }
  setupTracerClass(module);
  setupSpanClass(module);
  return module;
}
} // extern "C"
