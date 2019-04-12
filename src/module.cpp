#include <Python.h>

#include "tracer.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// ModuleMethods
//--------------------------------------------------------------------------------------------------
static PyMethodDef ModuleMethods[] = {
  {nullptr, nullptr}
};

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
  /* auto module_dict = PyModule_GetDict(module); */
  /* setupTracerClass(module_dict); */
  return module;
}
} // extern "C"
