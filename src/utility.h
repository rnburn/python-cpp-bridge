#pragma once

namespace python_bridge_tracer {
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
} // namespace python_bridge_tracer
