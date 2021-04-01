#pragma once

//
// Created by Dave on 9/20/20.
//

#include <array>

#include "boost/di.hpp"


BOOST_DI_NAMESPACE_BEGIN
namespace extension {

// ---------------------------------------------------------------------------
// Takes variadic template pack from concepts::any_of<Ts..>> and gives you an array of std::type_index
template<typename T> struct is_any_of : std::false_type {};
template<typename... Ts> struct is_any_of<boost::ext::di::v1_2_0::concepts::any_of<Ts...>> : std::true_type
{
  static constexpr size_t size() { return sizeof...(Ts); }
  static constexpr std::array<std::type_index, sizeof...(Ts)> var_args_to_array_ids()
  {
    const std::array<std::type_index, sizeof...(Ts)> res {typeid(Ts)...};
    return res;
  }
};
template<typename T> constexpr bool is_any_of_v = is_any_of<T>::value;

// ---------------------------------------------------------------------------
// Specialization function for any_of<Ts...> to array
template<typename T>
typename std::enable_if<is_any_of_v<T>, std::array<std::type_index, is_any_of<T>::size() > >::type
get_type_index_array()
{
  return is_any_of<T>::var_args_to_array_ids();
}

// Specialization function for just a type
template<typename T>
typename std::enable_if<!is_any_of_v<T> /*&& std::is_integral<T>()*/, std::array<std::type_index, 1> >::type
get_type_index_array()
{
  return std::array<std::type_index, 1>{ typeid(T) };
}

// ---------------------------------------------------------------------------
// Specialization function for any_of<Ts...> to array
template<typename T>
typename std::enable_if<is_any_of_v<T>, size_t >::type
constexpr get_size_index_array()
{
  return is_any_of<T>::size();
}

// Specialization function for just a type
template<typename T>
typename std::enable_if<!is_any_of_v<T> /*&& std::is_integral<T>()*/, size_t >::type
constexpr get_size_index_array()
{
  return 1;
}




} // namespace extension
BOOST_DI_NAMESPACE_END
