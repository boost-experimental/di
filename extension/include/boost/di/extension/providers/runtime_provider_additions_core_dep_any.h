#pragma once
//
// Created by Dave on 9/20/20.
//
#include <array>
#include "boost/di/extension/providers/runtime_provider_additions_any_of.h"

// ---------------------------------------------------------------------------
// di core dep any
template<typename, typename = void, typename = void, typename = void>
struct dwk_is_di_core_dep : std::false_type {};
template <typename SCOPE, typename BINDINGS, typename BOUND, typename W, typename X>
struct dwk_is_di_core_dep<boost::ext::di::v1_2_0::core::dependency<SCOPE, BINDINGS, BOUND, W, X>> : std::true_type
{
    typedef SCOPE scope_t; // scope: singleton?
    typedef BINDINGS bindings_t;
    typedef BOUND bound_t;
    typedef W W_t;
    typedef X X_t;

    static constexpr size_t size() { return boost::di::extension::get_size_index_array<bindings_t>(); }
    typedef std::array<std::type_index, size()> array_t;
};
template <typename T>
static constexpr bool dwk_is_di_core_dep_any_v = dwk_is_di_core_dep<T>::value;
