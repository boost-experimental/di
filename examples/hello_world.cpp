//
// Copyright (c) 2012 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/shared_ptr.hpp>
#include <di/di.hpp>

class C1 { };
class C2 { };

class LValue
{
public:
    DI_CTOR(LValue, C1, C2)
    { }
};

class SharedPtr
{
public:
    DI_CTOR(SharedPtr, boost::shared_ptr<C1>, boost::shared_ptr<C2>)
    { }
};

int main()
{
    di::Utility::Injector<> injector;
    injector.create<LValue>();
    injector.create<SharedPtr>();

    return 0;
}

