//
// Copyright (c) 2014 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "boost/di/cpp_0x/scopes/deduce.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/type_traits/is_same.hpp>

#include "boost/di/cpp_0x/aux_/config.hpp"
#include "boost/di/cpp_0x/aux_/memory.hpp"
#include "boost/di/cpp_0x/scopes/external.hpp"
#include "boost/di/cpp_0x/scopes/unique.hpp"
#include "boost/di/cpp_0x/scopes/shared.hpp"
#include "boost/di/cpp_0x/named.hpp"

namespace boost {
namespace di {
namespace scopes {

struct c { };

BOOST_AUTO_TEST_CASE(types) {
    BOOST_CHECK((is_same<unique<>, deduce::rebind<int>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<named<int>>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<c>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<named<c>>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<const int&>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<named<const int&>>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<int*>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<named<int*>>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<const int*>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<named<const int*>>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<aux::auto_ptr<int>>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<named<aux::auto_ptr<int>>>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<std::shared_ptr<int>>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<const std::shared_ptr<int>&>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<named<std::shared_ptr<int>>>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<const named<std::shared_ptr<int>>&>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<boost::shared_ptr<int>>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<const boost::shared_ptr<int>&>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<named<boost::shared_ptr<int>>>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<const named<boost::shared_ptr<int>>&>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<std::weak_ptr<int>>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<const std::weak_ptr<int>&>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<named<std::weak_ptr<int>>>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<const named<std::weak_ptr<int>>&>::other>::value));
    BOOST_CHECK((is_same<external<>, deduce::rebind<int&>::other>::value));
    BOOST_CHECK((is_same<external<>, deduce::rebind<named<int&>>::other>::value));
    BOOST_CHECK((is_same<external<>, deduce::rebind<const named<int&>&>::other>::value));

#if !defined(BOOST_NO_CXX11_SMART_PTR)
    BOOST_CHECK((is_same<unique<>, deduce::rebind<std::unique_ptr<int>>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<named<std::unique_ptr<int>>>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<std::shared_ptr<int>>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<named<std::shared_ptr<int>>>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<std::weak_ptr<int>>::other>::value));
    BOOST_CHECK((is_same<shared<>, deduce::rebind<named<std::weak_ptr<int>>>::other>::value));
#endif

#if !defined(BOOST_NO_RVALUE_REFERENCES)
    BOOST_CHECK((is_same<unique<>, deduce::rebind<int&&>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<named<int&&>>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<const int&&>::other>::value));
    BOOST_CHECK((is_same<unique<>, deduce::rebind<named<const int&&>>::other>::value));
#endif
}

} // namespace scopes
} // namespace di
} // namespace boost

