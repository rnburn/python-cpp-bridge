#include <iostream>

#include <Python.h>

#include "python_bridge_tracer/module.h"

#include "dynamic_tracer.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// loadTracer
//--------------------------------------------------------------------------------------------------
static PyObject* loadTracer(PyObject* /*self*/, PyObject* args, PyObject* keywords) noexcept try {
  static char* keyword_names[] = {const_cast<char*>("library"),
                                  const_cast<char*>("config"),
                                  const_cast<char*>("scope_manager"), nullptr};
  char* library;
  char* config;
  PyObject* scope_manager = nullptr;
  if (!PyArg_ParseTupleAndKeywords(args, keywords, "ss|O:load_tracer", keyword_names, 
        &library, &config, &scope_manager)) {
    return nullptr;
  }
  return makeTracer(makeDynamicTracer(library, config));
} catch(const std::exception& e) {
  std::cerr << "failed to load tracer: " << e.what() << "\n";
  // TODO: make exception
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// ModuleMethods
//--------------------------------------------------------------------------------------------------
static PyMethodDef ModuleMethods[] = {
    {"load_tracer", reinterpret_cast<PyCFunction>(loadTracer),
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
  if (!setupClasses(module)) {
    return nullptr;
  }
  return module;
}
} // extern "C"
