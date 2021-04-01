#pragma once
// DWK for this entire file

#include "boost/di.hpp"
#include "boost/di/extension/providers/runtime_provider_additions_core_dep_any.h"

BOOST_DI_NAMESPACE_BEGIN
namespace extension {

// ---------------------------------------------------------------------------
// scope_is_singleton
template<typename T>
typename std::enable_if<!std::is_same<boost::ext::di::v1_2_0::scopes::singleton, T>::value, bool>::type
scope_is_singleton() { return false; }

template<typename T>
typename std::enable_if<std::is_same<boost::ext::di::v1_2_0::scopes::singleton, T>::value, bool>::type
scope_is_singleton() { return true; }

// ---------------------------------------------------------------------------
// dependency_is_singleton_scope
template <typename T>
typename std::enable_if<!dwk_is_di_core_dep_any_v<T>, bool>::type
dependency_is_singleton_scope()
{
  return false; // Unknown really
}
template <typename T>
typename std::enable_if<dwk_is_di_core_dep_any_v<T>, bool>::type
dependency_is_singleton_scope()
{
  typedef typename dwk_is_di_core_dep<T>::scope_t scope_t;
  return scope_is_singleton<scope_t>();
}


} // namespace extension
BOOST_DI_NAMESPACE_END