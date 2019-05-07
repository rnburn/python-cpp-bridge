#include "tracer_bridge.h"

#include "span.h"
#include "span_context.h"
#include "utility.h"

#include "python_bridge_tracer/module.h"

namespace python_bridge_tracer {
//--------------------------------------------------------------------------------------------------
// getNumReferences
//--------------------------------------------------------------------------------------------------
static bool getNumReferences(PyObject* references, int& num_references) noexcept {
  if (references == nullptr || references == Py_None) {
    num_references = 0;
    return true;
  }
  if (PyList_Check(references) == 0) {
    PyErr_Format(PyExc_TypeError,
                 "references must be a list");
    return false;
  }
  num_references = PyList_Size(references);
  return true;
}

//--------------------------------------------------------------------------------------------------
// addParentReference
//--------------------------------------------------------------------------------------------------
static bool addParentReference(PyObject* parent, 
    std::vector<std::pair<opentracing::SpanReferenceType, SpanContextBridge>>&
        cpp_references) noexcept {
  if (parent == nullptr) {
    return true;
  }
  if (isSpanContext(parent)) {
    cpp_references.emplace_back(opentracing::SpanReferenceType::ChildOfRef,
                                getSpanContext(parent));
    return true;
  }
  if (isSpan(parent)) {
    cpp_references.emplace_back(opentracing::SpanReferenceType::ChildOfRef,
                                getSpanContextFromSpan(parent));
    return true;
  }
  PyErr_Format(PyExc_TypeError,
               "child_of must be either a " PYTHON_BRIDGE_TRACER_MODULE
               "._SpanContext or a " PYTHON_BRIDGE_TRACER_MODULE "._Span");
  return false;
}

//--------------------------------------------------------------------------------------------------
// addActiveSpanReference
//--------------------------------------------------------------------------------------------------
static bool addActiveSpanReference(PyObject* scope_manager,
    std::vector<std::pair<opentracing::SpanReferenceType, SpanContextBridge>>&
        cpp_references) noexcept {
  auto active_scope = PyObject_GetAttrString(scope_manager, "active");
  if (active_scope == nullptr) {
    return false;
  }
  auto cleanup_active_scope = finally([active_scope] {
      Py_DECREF(active_scope);
  });
  if (active_scope == Py_None) {
    return true;
  }
  auto active_span = PyObject_GetAttrString(active_scope, "span");
  if (active_span == nullptr) {
    return false;
  }
  auto cleanup_active_span = finally([active_span] {
      Py_DECREF(active_span);
  });
  if (!isSpan(active_span)) {
    PyErr_Format(
        PyExc_TypeError,
        "unexpected type for active span: expected " PYTHON_BRIDGE_TRACER_MODULE
        "._Span");
    return false;
  }
  cpp_references.emplace_back(opentracing::SpanReferenceType::ChildOfRef,
                              getSpanContextFromSpan(active_span));
  return true; 
}

//--------------------------------------------------------------------------------------------------
// getReferenceType
//--------------------------------------------------------------------------------------------------
static bool getReferenceType(
    PyObject* reference_type,
    opentracing::SpanReferenceType& cpp_reference_type) noexcept {
  if (PyUnicode_Check(reference_type) == 0) {
    PyErr_Format(PyExc_TypeError, "reference_type must be a string");
    return false;
  }
  auto utf8 = PyUnicode_AsUTF8String(reference_type);
  if (utf8 == nullptr) {
    return false;
  }
  auto cleanup_utf8 = finally([utf8] { Py_DECREF(utf8); });
  char* s;
  Py_ssize_t length;
  auto rcode = PyBytes_AsStringAndSize(utf8, &s, &length);
  if (rcode == -1) {
    return false;
  }
  auto string_view = opentracing::string_view{s, static_cast<size_t>(length)};
  static opentracing::string_view child_of{"child_of"};
  static opentracing::string_view follows_from{"follows_from"};
  if (string_view == child_of) {
    cpp_reference_type = opentracing::SpanReferenceType::ChildOfRef;
    return true;
  }
  if (string_view == follows_from) {
    cpp_reference_type = opentracing::SpanReferenceType::FollowsFromRef;
    return true;
  }
  PyErr_Format(PyExc_RuntimeError,
               "reference_type must be either 'child_of' or 'follows_from'");
  return false;
}

//--------------------------------------------------------------------------------------------------
// addReference
//--------------------------------------------------------------------------------------------------
static bool addReference(PyObject* reference, 
    std::vector<std::pair<opentracing::SpanReferenceType, SpanContextBridge>>&
        cpp_references) noexcept {
  auto reference_type = PyObject_GetAttrString(reference, "type");
  if (reference_type == nullptr) {
    return false;
  }
  auto cleanup_reference_type = finally([reference_type] {
      Py_DECREF(reference_type);
  });
  opentracing::SpanReferenceType cpp_reference_type;
  if (!getReferenceType(reference_type, cpp_reference_type)) {
    return false;
  }
  auto span_context = PyObject_GetAttrString(reference, "referenced_context");
  if (span_context == nullptr) {
    return false;
  }
  if (!isSpanContext(span_context)) {
    PyErr_Format(
        PyExc_TypeError,
        "unexpected type for referenced_context: expected " PYTHON_BRIDGE_TRACER_MODULE
        "._SpanContext");
    return false;
  }
  cpp_references.emplace_back(cpp_reference_type, getSpanContext(span_context));
  return true;
}

//--------------------------------------------------------------------------------------------------
// addReferences
//--------------------------------------------------------------------------------------------------
static bool addReferences(
    PyObject* references, int num_references, 
    std::vector<std::pair<opentracing::SpanReferenceType, SpanContextBridge>>&
        cpp_references) noexcept {
  for (int i=0; i<num_references; ++i) {
    auto reference = PyList_GetItem(references, i);
    if (!addReference(reference, cpp_references)) {
      return false;
    }
  }
  return true;
}

//--------------------------------------------------------------------------------------------------
// getCppReferences
//--------------------------------------------------------------------------------------------------
static bool getCppReferences(
    PyObject* scope_manager, PyObject* parent, PyObject* references,
    bool ignore_active_span,
    std::vector<std::pair<opentracing::SpanReferenceType, SpanContextBridge>>&
        cpp_references) noexcept {
  int num_references;
  if (!getNumReferences(references, num_references)) {
    return false;
  }
  cpp_references.reserve(static_cast<size_t>(num_references+2));
  if(!addParentReference(parent, cpp_references)) {
    return false;
  }
  if (!ignore_active_span) {
    if (!addActiveSpanReference(scope_manager, cpp_references)) {
      return false;
    }
  }
  return addReferences(references, num_references, cpp_references);
}

//--------------------------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------------------------
TracerBridge::TracerBridge(std::shared_ptr<opentracing::Tracer> tracer) noexcept
    : tracer_{std::move(tracer)} {}

//--------------------------------------------------------------------------------------------------
// makeSpan
//--------------------------------------------------------------------------------------------------
std::unique_ptr<SpanBridge> TracerBridge::makeSpan(
    opentracing::string_view operation_name, PyObject* scope_manager,
    PyObject* parent, PyObject* references, PyObject* tags, double start_time,
    bool ignore_active_span) noexcept {
  std::vector<std::pair<opentracing::SpanReferenceType, SpanContextBridge>>
      cpp_references;
  if (!getCppReferences(scope_manager, parent, references, ignore_active_span,
                        cpp_references)) {
    return nullptr;
  }
  opentracing::StartSpanOptions options;
  options.references.reserve(cpp_references.size());
  for (auto& reference : cpp_references) {
    options.references.emplace_back(reference.first,
                                    &reference.second.span_context());
  }
  (void)tags;
  (void)start_time;
  auto span = tracer_->StartSpanWithOptions(operation_name, options);
  return std::unique_ptr<SpanBridge>{new SpanBridge{std::move(span)}};
}
} // namespace python_bridge_tracer
