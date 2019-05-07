#include "span_bridge.h"

#include "utility.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// setStringTag
//--------------------------------------------------------------------------------------------------
static PyObject* setStringTag(opentracing::Span& span, opentracing::string_view key,
    PyObject* value) noexcept {
  auto utf8 = PyUnicode_AsUTF8String(value);
  if (utf8 == nullptr) {
    return nullptr;
  }
  auto cleanup_utf8 = finally([utf8] {
      Py_DECREF(utf8);
  });
  char* s;
  auto rcode = PyBytes_AsStringAndSize(utf8, &s, nullptr);
  if (rcode == -1) {
    return nullptr;
  }
  span.SetTag(key, s);
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------------------------
SpanBridge::SpanBridge(std::unique_ptr<opentracing::Span>&& span) noexcept
  : span_{span.release()} 
{}

SpanBridge::SpanBridge(std::shared_ptr<opentracing::Span> span) noexcept
    : span_{std::move(span)} {}

//--------------------------------------------------------------------------------------------------
// setTag
//--------------------------------------------------------------------------------------------------
PyObject* SpanBridge::setTag(PyObject* args, PyObject* keywords) noexcept {
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
    return setStringTag(
        *span_, opentracing::string_view{key, static_cast<size_t>(key_length)},
        value);
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
    PyErr_Format(PyExc_TypeError,
                 "tag value must be a string, bool, or a numeric type");
    return nullptr;
  }
  span_->SetTag(opentracing::string_view{key, static_cast<size_t>(key_length)},
               cpp_value);
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// finish
//--------------------------------------------------------------------------------------------------
PyObject* SpanBridge::finish(PyObject* args, PyObject* keywords) noexcept {
  (void)args;
  (void)keywords;
  span_->Finish();
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// exit
//--------------------------------------------------------------------------------------------------
PyObject* SpanBridge::exit(PyObject* args) noexcept {
  (void)args;
  span_->Finish();
  Py_RETURN_NONE;
}
}  // namespace python_bridge_tracer
