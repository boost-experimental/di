#pragma once
// DWK for this entire file

#include <cassert>
#include <functional>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include "boost/di.hpp"
#include "boost/di/extension/scopes/shared.hpp"

#include "boost/di/extension/providers/runtime_provider_additions_any_of.h"
#include "boost/di/extension/providers/runtime_provider_additions_di_core_array.h"
//#include "boost/di/extension/providers/runtime_provider
#include "boost/di/extension/providers/runtime_provider_generator_saver.h"
#include "boost/di/extension/providers/runtime_provider_additions_is_singleton.h"


BOOST_DI_NAMESPACE_BEGIN
namespace extension {


// ---------------------------------------------------------------------------
// dependency_any_to_type_index_vec
template <typename T>
typename std::enable_if<!dwk_is_di_core_dep_any_v<T>, std::array<std::type_index, 0> >::type
constexpr dependency_any_to_type_index_vec() // std::vector<std::type_index> & /*type_index_vec*/)
{
  // TODO, assert??  is there really nothing to add to the vector
  return std::array<std::type_index, 0>();
}



template <typename T>
typename std::enable_if<dwk_is_di_core_dep_any_v<T>, typename dwk_is_di_core_dep<T>::array_t >::type
constexpr dependency_any_to_type_index_vec()
{
//    std::cout << "----------------------------\n";
//    std::cout << "dwk_get_binding_typeids di_Core template\n";
//    std::cout << "SCOPE>>> " << typeid(typename dwk_is_di_core_dep<T>::scope_t).name() << std::endl;
//    std::cout << "BINDINGS>>> " << typeid(typename dwk_is_di_core_dep<T>::bindings_t).name() << std::endl;
//    std::cout << "BOUND>>> " << typeid(typename dwk_is_di_core_dep<T>::bound_t).name() << std::endl;
//    std::cout << "W>>> " << typeid(typename dwk_is_di_core_dep<T>::W_t).name() << std::endl;
//    std::cout << "X>>> " << typeid(typename dwk_is_di_core_dep<T>::X_t).name() << std::endl;
//    std::cout << "----------------------------\n";

  // TODO, link up dwk_is_di_core_dep and this method get_type_index_array insead of them just happening to have the same return type '1' currently
  const auto res = get_type_index_array<typename dwk_is_di_core_dep<T>::bindings_t>();
  return res;
}


#if 0
// TODO, or maybe just delete all of this?
// ---------------------------------------------------------------------------
// TODO, this needs some cleanup
template <typename T, typename DATA_T, typename MADE_T>
//typename std::enable_if<dwk_is_core_any_v<T>, void>::type
void
dwk_add_to_data2(const bool is_singleton, DATA_T & data, const std::vector<std::type_index> & type_index_vec, MADE_T m)
{
  if(!is_singleton)
  {
    std::cout << "Skipping adding non-singleton " << typeid(T).name() << std::endl;
     return;
  }
//      std::cout << "ADD TO DATA2 CRAZY MULTI INHERITANCE " << typeid(T).name() << "\n";
  // only create this shared_ptr once and stuff it everywhere it has been bound to
  std::shared_ptr<typename std::remove_pointer<MADE_T>::type> made_shared_ptr{m};
  std::shared_ptr<void> void_shared_ptr = made_shared_ptr;

  // TODO, don't need to pass in the type index vec cause we can just get it from the dwk_is_core_any<T>::??
  for(auto & type_idx : type_index_vec)
  {
    std::cout << "   ADDING to data: " << typeid(MADE_T).name() << " as a " << type_idx.name() << ", " << type_idx.hash_code() << "\n";
    data[type_idx].push_back(void_shared_ptr);
    std::cout << "data[type_idx].size() == " << data[type_idx].size() << std::endl;
//          std::cout << "   ADDING after size = " << data[type_idx].size() << "\n";
//          std::cout << std::endl;
  }
}

template <typename T, typename DATA_T, typename MADE_T>
typename std::enable_if<!dwk_is_di_core_dep_any_v<T>, void>::type
dwk_add_to_data(const bool /*is_singleton*/, DATA_T & /*data*/, const std::vector<std::type_index> & /*type_index_vec*/, MADE_T /*m*/)
{
  // TODO, believe this is done with double, int, etc.
  std::cout << "dwk_add_to_data nada" << std::endl;
}

template <typename T, typename DATA_T, typename MADE_T>
// DWK, TODO make this also ensure that T::given is a pointer and not an instance
typename std::enable_if<dwk_is_di_core_dep_any_v<T>, void>::type
dwk_add_to_data(const bool is_singleton, DATA_T & data, const std::vector<std::type_index> & type_index_vec, MADE_T m)
{
  typedef typename dwk_is_di_core_dep<T>::bindings_t bindings_t;
  dwk_add_to_data2<bindings_t, DATA_T>(is_singleton, data, type_index_vec, m);
}

#endif

} // namespace extension
BOOST_DI_NAMESPACE_END
