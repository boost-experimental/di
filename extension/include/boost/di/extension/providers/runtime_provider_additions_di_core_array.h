#pragma once
// DWK for this entire file

#include "boost/di.hpp"
#include "boost/di/extension/scopes/shared.hpp"

BOOST_DI_NAMESPACE_BEGIN
namespace extension {

// ---------------------------------------------------------------------------
// This specialization detects when we're trying to create an std::vector<std::shared_ptr<interface_t>>
// it's not just a vector, it's some weird boost di wrapper core array with a vector inside that
//    template<typename T>
//    struct is_vector : public std::false_type {};
//    template<typename T>
//    struct is_vector<std::vector<T>> : public std::true_type {};

template <typename T>
struct is_di_core_array : public std::false_type {};
template <typename T> //                N5boost3ext2di6v1_2_04core5arrayIFNSt3__16vectorINS5_10shared_ptrI2i1EENS5_9allocatorIS9_EEEEvEJEEE
struct is_di_core_array<boost::ext::di::v1_2_0::core::array<std::__1::vector<T*, std::__1::allocator<T*> > ()> > : public std::true_type
{
typedef T subtype;
typedef T* raw_ptr_t;
typedef T* ptr_t;
};
// but then shared_ptr<T> is a possibility too - why don't we just tie into the other get<T> creation methods?
template <typename T>
struct is_di_core_array<boost::ext::di::v1_2_0::core::array<std::__1::vector<std::shared_ptr<T>, std::__1::allocator<std::shared_ptr<T>> > ()> > : public std::true_type
{
typedef T subtype;
typedef T* raw_ptr_t;
typedef std::shared_ptr<T> ptr_t;
};


// ----------------------------------------------------------------------------
template <typename T>
struct is_di_core_array_list : public std::false_type {};
template <typename T>                       // boost::ext::di::v1_2_0::cores::successful::any_type_ref<std::__1::list, boost::ext::di::v1_2_0::core::injector>
                       //get<T> NSt3__14listINS_10shared_ptrI2i1EENS_9allocatorIS3_EEEE
struct is_di_core_array_list<std::__1::list<T*, std::__1::allocator<T*> >  > : public std::true_type
{
  typedef T subtype;
  typedef T* raw_ptr_t;
  typedef T* ptr_t;
};
// but then shared_ptr<T> is a possibility too - why don't we just tie into the other get<T> creation methods?
template <typename T>
struct is_di_core_array_list<std::__1::list<std::shared_ptr<T>, std::__1::allocator<std::shared_ptr<T>> >  > : public std::true_type
{
  typedef T subtype;
  typedef T* raw_ptr_t;
  typedef std::shared_ptr<T> ptr_t;
};



} // namespace extension
BOOST_DI_NAMESPACE_END
