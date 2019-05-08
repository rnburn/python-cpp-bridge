#include "span_bridge.h"

#include "utility.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// setStringTag
//--------------------------------------------------------------------------------------------------
static bool setStringTag2(opentracing::Span& span, opentracing::string_view key,
    PyObject* value) noexcept {
  auto utf8 = PyUnicode_AsUTF8String(value);
  if (utf8 == nullptr) {
    return false;
  }
  auto cleanup_utf8 = finally([utf8] {
      Py_DECREF(utf8);
  });
  char* s;
  auto rcode = PyBytes_AsStringAndSize(utf8, &s, nullptr);
  if (rcode == -1) {
    return false;
  }
  span.SetTag(key, s);
  return true;
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
// setOperationName
//--------------------------------------------------------------------------------------------------
PyObject* SpanBridge::setOperationName(PyObject* args,
                                       PyObject* keywords) noexcept {
  static char* keyword_names[] = {const_cast<char*>("operation_name"), nullptr};
  const char* operation_name = nullptr;
  int operation_name_length = 0;
  if (!PyArg_ParseTupleAndKeywords(args, keywords, "s#:set_operation_name",
                                   keyword_names, &operation_name,
                                   &operation_name_length)) {
    return nullptr;
  }
  span_->SetOperationName(opentracing::string_view{
      operation_name, static_cast<size_t>(operation_name_length)});
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// setTagKeyValue
//--------------------------------------------------------------------------------------------------
bool SpanBridge::setTagKeyValue(opentracing::string_view key, PyObject* value) noexcept {
  opentracing::Value cpp_value;
  if (PyUnicode_Check(value) == 1) {
    return setStringTag2(*span_, key, value);
  } else if (PyBool_Check(value) == 1) {
    cpp_value = static_cast<bool>(PyObject_IsTrue(value));
  } else if (PyLong_Check(value) == 1) {
    auto long_value = PyLong_AsLong(value);
    if (long_value == -1 && PyErr_Occurred()) {
      return false;
    }
    cpp_value = long_value;
  } else if (PyFloat_Check(value) == 1) {
    auto double_value = PyFloat_AsDouble(value);
    if (PyErr_Occurred()) {
      return false;
    }
    cpp_value = double_value;
  } else {
    PyErr_Format(PyExc_TypeError,
                 "tag value must be a string, bool, or a numeric type");
    return false;
  }
  span_->SetTag(key, cpp_value);
  return true;
}

bool SpanBridge::setTagKeyValue(PyObject* key, PyObject* value) noexcept {
  if (PyUnicode_Check(key) != 1) {
    PyErr_Format(PyExc_TypeError, "tag key must be a string");
    return false;
  }
  auto utf8 = PyUnicode_AsUTF8String(key);
  if (utf8 == nullptr) {
    return false;
  }
  auto cleanup_utf8 = finally([utf8] {
      Py_DECREF(utf8);
  });
  char* key_data;
  Py_ssize_t key_length;
  auto rcode = PyBytes_AsStringAndSize(utf8, &key_data, &key_length);
  if (rcode == -1) {
    return false;
  }
  return setTagKeyValue(opentracing::string_view{key_data, static_cast<size_t>(key_length)}, value);
}

//--------------------------------------------------------------------------------------------------
// setTag
//--------------------------------------------------------------------------------------------------
PyObject* SpanBridge::setTag(PyObject* args, PyObject* keywords) noexcept {
  static char* keyword_names[] = {const_cast<char*>("key"),
                                  const_cast<char*>("value"),
                                  nullptr};
  const char* key_data;
  int key_length;
  PyObject* value;
  if (!PyArg_ParseTupleAndKeywords(args, keywords, "s#O:set_tag", keyword_names,
                                   &key_data, &key_length, &value)) {
    return nullptr;
  }
  if (!setTagKeyValue(opentracing::string_view{key_data, static_cast<size_t>(key_length)}, value)) {
    return nullptr;
  }
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// finish
//--------------------------------------------------------------------------------------------------
PyObject* SpanBridge::finish(PyObject* args, PyObject* keywords) noexcept {
  static char* keyword_names[] = {
    const_cast<char*>("finish_time"),
    nullptr
  };
  double finish_time = 0;
  if (!PyArg_ParseTupleAndKeywords(
        args, keywords, "|d:finish", keyword_names, &finish_time)) {
    return nullptr;
  }
  if (finish_time != 0) {
    auto time_since_epoch = std::chrono::nanoseconds{static_cast<uint64_t>(1e9*finish_time)};
    auto system_timestamp = std::chrono::system_clock::time_point{
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            time_since_epoch)};
    finish_span_options_.finish_steady_timestamp =
        opentracing::convert_time_point<std::chrono::steady_clock>(
            system_timestamp);
  }
  span_->FinishWithOptions(finish_span_options_);
  Py_RETURN_NONE;
}

//--------------------------------------------------------------------------------------------------
// exit
//--------------------------------------------------------------------------------------------------
PyObject* SpanBridge::exit(PyObject* args) noexcept {
  (void)args;
  // TODO: error processing
  span_->FinishWithOptions(finish_span_options_);
  Py_RETURN_NONE;
}
}  // namespace python_bridge_tracer
