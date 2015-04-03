Boost.DI: C++ Dependency Injection
===============================================

[![Join the chat at https://gitter.im/krzysztof-jusiak/di](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/krzysztof-jusiak/di?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Boost Libraries](https://raw.githubusercontent.com/krzysztof-jusiak/di/gh-pages/boost/boost.png)](http://www.boost.org)
[![Build Status](https://img.shields.io/travis/krzysztof-jusiak/di/cpp14.svg)](https://travis-ci.org/krzysztof-jusiak/di)
[![Coveralls](http://img.shields.io/coveralls/krzysztof-jusiak/di/cpp14.svg)](https://coveralls.io/r/krzysztof-jusiak/di?branch=cpp14)
[![Valgrind](https://img.shields.io/badge/valgrind-clean-brightgreen.svg)](https://travis-ci.org/krzysztof-jusiak/di)
[![Github Issues](https://img.shields.io/github/issues/krzysztof-jusiak/di.svg)](http://github.com/krzysztof-jusiak/di/issues)

[![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)](http://www.boost.org/LICENSE_1_0.txt)
[![Github Release](http://img.shields.io/github/release/krzysztof-jusiak/di.svg)](https://github.com/krzysztof-jusiak/di/releases/latest)

> "Don't call us, we'll call you", Hollywood principle

**Dependency Injection (DI)** involves passing (injecting) one or more dependencies (or services) to a dependent object (or client) which become part of the client’s state.
It is like the Strategy Pattern, except the strategy is set once, at construction. DI enables loosely coupled designs, which are easier to maintain and test.

> **"Let's make some coffee!"**
<p align="center"><img src="https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker.png" alt="coffee maker"/></p>
```cpp
No Dependency injection                 | Dependency Injection
----------------------------------------|--------------------------------------------
class coffee_maker {                    | class coffee_maker {
public:                                 | public:
    coffee_maker()                      |     coffee_maker(shared_ptr<iheater> heater
      : heater{                         |                , unique_ptr<ipump> pump)
          make_shared<electric_heater>()|         : heater(heater)
        }                               |         , pump(move(pump))
      , pump{                           |     { }
          make_unique<heat_pump>(heater)|
        }                               |     void brew() {
    { }                                 |         heater->on();
                                        |         pump->pump();
    void brew() {                       |         clog << "coffee!" << endl;
        heater->on();                   |         heater->off();
        pump->pump();                   |     }
        clog << "coffee"! << endl;      |
        heater->off();                  | private:
    }                                   |     shared_ptr<iheater> heater;
                                        |     unique_ptr<ipump> pump;
private:                                | };
    shared_ptr<iheater> heater;         |
    unique_ptr<ipump> pump;             |
};                                      |
```

**Why Dependency Injection?**

* DI provides loosely coupled code (separation of business logic and object creation)
* DI provides easier to maintain code (different objects might be easily injected)
* DI provides easier to test code (fakes objects might be injected)

[![The Clean Code Talks - Don't Look For Things!](http://img.youtube.com/vi/RlfLCWKxHJ0/0.jpg)](http://www.youtube.com/watch?v=RlfLCWKxHJ0) | [![DAGGER 2 - A New Type of dependency injection](http://image.slidesharecdn.com/nr73mexetqiybd1qpcem-140529143342-phpapp01/95/dependency-injection-for-android-5-638.jpg?cb=1401392113)](http://www.youtube.com/watch?v=oK_XtfXPkqw) |
--- | --- |

**Why Boost.DI?**

* Boost.DI has none or minimal run-time overhead - [Run-time performance](#run_time_performance)
* Boost.DI compiles fast - [Compile-time performance](#compile_time_performance)
* Boost.DI gives short diagnostic messages - [Diagnostic messages](#diagnostic_messages)

> **Boost.DI is not intrusive**
<p align="center"><img src="https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker_1.png" alt="coffee maker"/></p>
```cpp
Manual Dependency Injection             | Boost.DI (same as manual di)
----------------------------------------|-----------------------------------------
coffee_maker(shared_ptr<iheater> heater | coffee_maker(shared_ptr<iheater> heater
           , unique_ptr<ipump> pump);   |            , unique_ptr<ipump> pump);
```

*

> **Boost.DI reduces boilerplate code**
<p align="center"><img src="https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker_2.png" alt="coffee maker"/></p>
```cpp
Manual Dependency Injection             | Boost.DI
----------------------------------------|-----------------------------------------
int main() {                            | int main() {
   // has to be before pump             |     auto injector = di::make_injector(
   auto heater = shared_ptr<iheater>{   |         di::bind<ipump, heat_pump>
       make_shared<electric_heater>()   |       , di::bind<iheater, electric_heater>
   };                                   |     );
                                        |
   // has to be after heater            |     auto cm = injector.create<coffee_maker>();
   auto pump = unique_ptr<ipump>{       |     cm.brew();
       make_unique<heat_pump>(heater)   | }
   };                                   |
                                        |
   coffee_maker cm{heater, move(pump)}; |
   cm.brew();                           |
}                                       |
```
[![coffee maker](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker_3_cd.png)](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker_3_cd.png) | [![coffee maker](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker_3_od.png)](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker_3_od.png) |
--- | --- |
```cpp
Manual Dependency Injection             | Boost.DI (only 1 new binding)
----------------------------------------|-----------------------------------------
int main() {                            | int main() {
   // has to be before pump             |     auto injector = di::make_injector(
   auto heater = shared_ptr<iheater>{   |         di::bind<ipump, heat_pump>
       make_shared<electric_heater>()   |       , di::bind<iheater, electric_heater>
   };                                   |       , di::bind<igrinder, grinder> // new
                                        |     );
   // has to be before pump             |
   // and after heater                  |     auto cm = injector.create<coffee_maker>();
   auto grinder = unique_ptr<igrinder>{ |     cm.brew();
       make_unique<grinder>(heater)     | }
   };                                   |
                                        |
   // has to be after                   |
   // heater and grinder                |
   auto pump = unique_ptr<ipump>{       |
       make_unique<heat_pump>(          |
           heater, grinder              |
       )                                |
   };                                   |
                                        |
   coffee_maker cm{heater, move(pump)}; |
   cm.brew();                           |
}                                       |
```
[![coffee maker](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker_4_cd.png)](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker_4_cd.png) | [![coffee maker](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker_4_od.png)](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker_4_od.png) |
--- | --- |
```cpp
Manual Dependency Injection             | Boost.DI (no changes!)
----------------------------------------|-----------------------------------------
int main() {                            |
   // has to be before pump             |
   auto heater = shared_ptr<iheater>{   |
       make_shared<electric_heater>()   |
   };                                   |
                                        |
   // has to be after                   |
   // heater and grinder                |
   auto pump = unique_ptr<ipump>{       |
       make_unique<heat_pump>(          |
           heater                       |                    ✔
       )                                |
   };                                   |
                                        |
   // has to be before pump             |
   // and after heater                  |
   auto grinder = unique_ptr<igrinder>{ |
     make_unique<grinder>(heater, pump) |
   };                                   |
                                        |
   coffee_maker cm{heater, move(pump)}; |
   cm.brew();                           |
}                                       |
```
<p align="center"><img src="https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker_5.png" alt="coffee maker"/></p>
```cpp
Manual Dependency Injection             | Boost.DI (no changes!)
----------------------------------------|-----------------------------------------
int main() {                            |
   ...                                  |
   auto grinder = unique_ptr<igrinder>{ |                    ✔
     make_unique<grinder>(pump, heater) |
   };                                   |
   ...                                  |
}                                       |
```

*

> **Boost.DI reduces testing effort**
```cpp
Manual Dependency Injection             | Boost.DI
----------------------------------------|--------------------------------------------
auto heater_mock = create_heater_mock{};| auto injector = di::make_injector<mocks_provider>();
auto pump_mock = create_pump_mock{};    | auto cm = injector.create<coffee_maker>();
coffee_maker cm{heater_mock, pump_mock};| expect_call(&iheater::on);
expect_call(&ipump::pump);              | expect_call(&ipump::pump);
expect_call(&iheater::off);             | expect_call(&iheater::off);
cm.brew();                              | cm.brew();
```

*

> **Boost.DI gives better control of what and how is created**
```cpp
Manual Dependency Injection             | Boost.DI
----------------------------------------|--------------------------------------------
                                        | class allow_only_smart_ptrs : public di::config {
                                        | public:
                                        |   auto policies() const noexcept {
                                        |     return di::make_policies(
                                        |       constructible(
                                        |         is_smart_ptr<di::policies::_>{}
                  ?                     |     );
                                        |   }
                                        | };
                                        |
                                        | auto injector = di::make_injector();
                                        | injector.create<int>(); // compile error
                                        | injector.create<unique_ptr<int>>(); // okay
```

*

> **Boost.DI gives better understanding about objects hierarchy**
```cpp
Manual Dependency Injection             | Boost.DI
----------------------------------------|--------------------------------------------
                 ?                      | injector = di::make_injector<types_dumper>();
                                        | auto cm = injector.create<coffee_maker>();
----------------------------------------|--------------------------------------------
                                        | (coffee_maker)
                                        |     (shared_ptr<iheater> -> electric_heater)
                 ?                      |     (unique_ptr<ipump> -> heat_pump)
                                        |         (shared_ptr<iheater> -> electric_heater)
                                        |
```
<p align="center"><img src="https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/coffee_maker_uml.png" alt="coffee maker"/></p>

**How To Start?**

* Get C++14 compliant compiler (Clang-3.4+, GCC-5.0+) with STL (memory, type\_traits headers) / Boost is not required
* [Read Quick User Guide](#quick_user_guide)
* [Read Tutorial](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/tutorial.html)
* [Read Documentation](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html)

> To get started the only file you need is `di.hpp`:

[https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/include/boost/di.hpp](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/include/boost/di.hpp)

```cpp
    // main.cpp
    #include "di.hpp"
    int main() { }
```

```sh
    $CXX -std=c++1y -I. main.cpp
```

> To get and test Boost.DI library:

```sh
    git clone --recursive https://github.com/krzysztof-jusiak/di.git
    cd di/build
    ./bootstrap.sh --with-toolset=clang
    ./b2 -j4 ../test ../example
```

<a id="quick_user_guide"></a>
**Quick User Guide** | [Examples](https://github.com/krzysztof-jusiak/di/tree/cpp14/example/quick_user_guide)

* [Injector](#injector) | [Bindings](#bindings) | [Injections](#injections) | [Annotations](#annotations) | [Scopes](#scopes) | [Modules](#modules) | [Providers](#providers) | [Policies](#policies)
* [Run-time performance](#run_time_performance) | [Compile-time performance](#compile_time_performance)
* [Diagnostic messages](#diagnostic_messages) | [Configuration](#configuration)

> Let's assume all examples below include `boost/di.hpp` header and define `di` namespace alias.
```cpp
#include <boost/di.hpp>
namespace di = boost::di;
//
struct i1 { virtual ~i1() = default; virtual void dummy1() = 0; };
struct i2 { virtual ~i2() = default; virtual void dummy2() = 0; };
struct impl1 : i1 { void dummy1() override { } };
struct impl2 : i2 { void dummy2() override { } };
struct impl : i1, i2 { void dummy1() override { } void dummy2() override { } };
```

*
<a id="injector"></a>
> **[Injector](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/concepts/injector.html)**
```cpp
Create empty injector                   | Test
----------------------------------------|-----------------------------------------
auto injector = di::make_injector();    | assert(0 == injector.create<int>());
```

*

<a id="bindings"></a>
> **[Bindings](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/concepts/bindings.html)** | [Examples](https://github.com/krzysztof-jusiak/di/blob/cpp14/example/bindings.cpp) | [More examples](https://github.com/krzysztof-jusiak/di/blob/cpp14/example/dynamic_bindings.cpp)
```cpp
Bind interface to implementation        | Test
----------------------------------------|-----------------------------------------
auto injector = di::make_injector(      | auto object = injector.create<unique_ptr<i1>>();
    di::bind<i1, impl1>                 | assert(dynamic_cast<impl1*>(object.get()));
);                                      |
```
```cpp
Bind different interfaces to one        | Test
implementation                          |
----------------------------------------|-----------------------------------------
auto injector = di::make_injector(      | auto object1 = injector.create<shared_ptr<i1>>();
    di::bind<di::any_of<i1, i2>, impl>  | auto object2 = injector.create<shared_ptr<i2>>();
);                                      | assert(dynamic_cast<impl*>(object1.get()));
                                        | assert(dynamic_cast<impl*>(object2.get()));
```
```cpp
Bind type to compile time value         | Test
----------------------------------------|-----------------------------------------
template<int N> using int_ =            | assert(42 == injector.create<int>());
    integral_constant<int, N>;          |
                                        |
auto injector = di::make_injector(      |
    di::bind<int, int_<42>>             |
);                                      |
```
```cpp
Bind type to value (see external scope) | Test
----------------------------------------|-----------------------------------------
auto injector = di::make_injector(      | assert(42 == injector.create<int>());
    di::bind<int>.to(42)                |
);                                      |
```

*

<a id="injections"></a>
> **[Injections](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/concepts/injections.html)** | [Examples](https://github.com/krzysztof-jusiak/di/blob/cpp14/example/constructor_injection.cpp) | [More examples](https://github.com/krzysztof-jusiak/di/blob/cpp14/example/constructor_signature.cpp)
```cpp
Direct constructor injection            | Test
----------------------------------------|-----------------------------------------
struct c {                              | auto object = injector.create<c>();
    c(int a, double d) : a(a), d(d) { } | assert(42 == object.a);
                                        | assert(87.0 == object.d);
    int a = 0;                          |
    double d = 0.0;                     |
};                                      |
                                        |
auto injector = di::make_injector(      |
    di::bind<int>.to(42)                |
  , di::bind<double>.to(87.0)           |
);                                      |
```
```cpp
Aggregate constructor injection         | Test
----------------------------------------|-----------------------------------------
struct c {                              | auto object = injector.create<c>();
    int a = 0;                          | assert(42 == object.a);
    double d = 0.0;                     | assert(87.0 == object.d);
};                                      |
                                        |
auto injector = di::make_injector(      |
    di::bind<int>.to(42)                |
  , di::bind<double>.to(87.0)           |
);                                      |
```
```cpp
Direct constructor injection with many  | Test
constructors (longest parameters list   |
constructor will be chosen)             |
----------------------------------------|-----------------------------------------
struct c {                              | auto object = injector.create<c>();
    c();                                | assert(42 == object.a);
    c(int a) : a(a) { }                 | assert(87.0 == object.d);
    c(int a, double d) : a(a), d(d) { } |
                                        |
    int a = 0;                          |
    double d = 0.0;                     |
};                                      |
                                        |
auto injector = di::make_injector(      |
    di::bind<int>.to(42)                |
  , di::bind<double>.to(87.0)           |
);                                      |
```
```cpp
Direct constructor injection with       | Test
ambiguous constructors (BOOST_DI_INJECT)|
----------------------------------------|-----------------------------------------
struct c {                              | auto object = injector.create<c>();
    c(double d, int a) : a(a), d(d) { } | assert(42 == object.a);
    BOOST_DI_INJECT(c, int a, double d) | assert(87.0 == object.d);
        : a(a), d(d) { }                |
                                        |
    int a = 0;                          |
    double d = 0.0;                     |
};                                      |
                                        |
auto injector = di::make_injector(      |
    di::bind<int>.to(42)                |
  , di::bind<double>.to(87.0)           |
);                                      |
```
```cpp
Direct constructor injection with       | Test
ambiguous constructors                  |
(BOOST_DI_INJECT_TRAITS)                |
----------------------------------------|-----------------------------------------
struct c {                              | auto object = injector.create<c>();
    BOOST_DI_INJECT_TRAITS(int, double);| assert(42 == object.a);
    c(double d, int a) : a(a), d(d) { } | assert(87.0 == object.d);
    c(int a, double d) : a(a), d(d) { } |
                                        |
    int a = 0;                          |
    double d = 0.0;                     |
};                                      |
                                        |
auto injector = di::make_injector(      |
    di::bind<int>.to(42)                |
  , di::bind<double>.to(87.0)           |
);                                      |
```
```cpp
Direct constructor injection with       | Test
ambiguous constructors                  |
(di::ctor_traits)                       |
----------------------------------------|-----------------------------------------
struct c {                              | auto object = injector.create<c>();
    c(double d, int a) : a(a), d(d) { } | assert(42 == object.a);
    c(int a, double d) : a(a), d(d) { } | assert(87.0 == object.d);
                                        |
    int a = 0;                          |
    double d = 0.0;                     |
};                                      |
                                        |
namespace boost { namespace di {        |
template<>                              |
struct ctor_traits<c> {                 |
    BOOST_DI_INJECT_TRAITS(int, double);|
};                                      |
}} // boost::di                         |
                                        |
auto injector = di::make_injector(      |
    di::bind<int>.to(42)                |
  , di::bind<double>.to(87.0)           |
);                                      |
```

*

<a id="annotations"></a>
> **[Annotations](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/concepts/annotations.html)** | [Examples](https://github.com/krzysztof-jusiak/di/blob/cpp14/example/annotations.cpp)
```cpp
Annotated constructor injection         | Test
----------------------------------------|-----------------------------------------
auto int1 = []{};                       | auto object = injector.create<c>();
auto int2 = []{};                       | assert(42 == object.a);
                                        | assert(87 == object.b);
struct c {                              |
    BOOST_DI_INJECT(c                   |
        , (named = int1) int a          |
        , (named = int2) int b)         |
        : a(a), b(b)                    |
    { }                                 |
                                        |
    int a = 0;                          |
    int b = 0;                          |
};                                      |
                                        |
auto injector = di::make_injector(      |
    di::bind<int>.named(int1).to(42)    |
  , di::bind<int>.named(int2).to(87)    |
);                                      |
```
```cpp
Annotated constructor injection with    | Test
the same names for different parameters |
----------------------------------------|-----------------------------------------
auto n1 = []{};                         | auto object = injector.create<c>();
auto n2 = []{};                         | assert(42 == object.i1);
                                        | assert(42 == object.i2);
struct c {                              | assert(87 == object.i3);
  BOOST_DI_INJECT(c                     | assert(0 == object.i4);
      , (named = n1) int a              | assert("str" == c.s);
      , (named = n1) int b              |
      , (named = n2) int c              |
      , int d                           |
      , (named = n1) string s)          |
  : i1(i1), i2(i2), i3(i3), i4(i4), s(s)|
  { }                                   |
                                        |
  int i1 = 0;                           |
  int i2 = 0;                           |
  int i3 = 0;                           |
  int i4 = 0;                           |
  string s;                             |
};                                      |
                                        |
auto injector = di::make_injector(      |
    di::bind<int>.named(n1).to(42)      |
  , di::bind<int>.named(n2).to(87)      |
  , di::bind<string>.named(n1).to("str")|
);                                      |
```
```cpp
Annotated constructor injection with    | Test
separate constructor definition         |
----------------------------------------|-----------------------------------------
auto int1 = []{};                       | auto object = injector.create<c>();
auto int2 = []{};                       | assert(42 == object.a);
                                        | assert(87 == object.b);
struct c {                              |
    BOOST_DI_INJECT(c                   |
        , (named = int1) int a          |
        , (named = int2) int b);        |
                                        |
    int a = 0;                          |
    int b = 0;                          |
};                                      |
                                        |
c::c(int a, int b) : a(a), b(b) { }     |
                                        |
auto injector = di::make_injector(      |
    di::bind<int>.named(int1).to(42)    |
  , di::bind<int>.named(int2).to(87)    |
);                                      |
```
```cpp
Annotated constructor injection with    | Test
di::ctor_traits                         |
----------------------------------------|-----------------------------------------
auto int1 = []{};                       | auto object = injector.create<c>();
auto int2 = []{};                       | assert(42 == object.a);
                                        | assert(87 == object.b);
struct c {                              |
    c(int a, int b) : a(a), b(b) { }    |
                                        |
    int a = 0;                          |
    int b = 0;                          |
};                                      |
                                        |
namespace boost { namespace di {        |
template<>                              |
struct ctor_traits<c> {                 |
    BOOST_DI_INJECT_TRAITS(             |
        (named = int1) int              |
      , (named = int2) int);            |
};                                      |
}} // boost::di                         |
                                        |
auto injector = di::make_injector(      |
    di::bind<int>.named(int1).to(42)    |
  , di::bind<int>.named(int2).to(87)    |
);                                      |
```

*

<a id="scopes"></a>
> **[Scopes](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/concepts/scopes.html)** | [Examples](https://github.com/krzysztof-jusiak/di/blob/cpp14/example/deduce_scope.cpp) | [More examples](https://github.com/krzysztof-jusiak/di/blob/cpp14/example/custom_scope.cpp)
```cpp
Deduce scope (default)                  | Test
----------------------------------------|-----------------------------------------
struct c {                              | auto object1 = injector.create<unique_ptr<c>>();
    shared_ptr<i1> sp; /*singleton*/    | auto object2 = injector.create<unique_ptr<c>>();
    unique_ptr<i2> up; /*unique*/       | assert(object1->sp == object2->sp);
    int& i; /*external*/                | assert(object1->up != object2->up);
    double d; /*unique*/                | assert(42 == object1->i);
};                                      | assert(&i == &object1->i;
                                        | assert(42 == object2->i);
auto i = 42;                            | assert(&i == &object2->i);
                                        | assert(87.0 == object1->d);
auto injector = di::make_injector(      | assert(87.0 == object2->d);
    di::bind<i1, impl1>                 |
  , di::bind<i2, impl2>                 |
  , di::bind<int>.to(ref(i))            |
  , di::bind<double>.to(87.0)           |
);                                      |
```
| Type | Deduced scope |
|------|-------|
| T | unique |
| T& | error - has to be bound as external |
| const T& | unique (temporary) |
| T* | unique (ownership transfer) |
| const T* | unique (ownership transfer) |
| T&& | unique |
| unique\_ptr<T> | unique |
| shared\_ptr<T> | singleton |
| weak\_ptr<T> | singleton |
```cpp
Unique scope                            | Test
----------------------------------------|-----------------------------------------
auto injector = di::make_injector(      | assert(injector.create<shared_ptr<i1>>()
    di::bind<i1, impl1>.in(di::unique)  |        !=
);                                      |        injector.create<shared_ptr<i1>>()
                                        | );
```
```cpp
Shared scope (shared per one thread)    | Test
----------------------------------------|-----------------------------------------
auto injector = di::make_injector(      | assert(injector.create<shared_ptr<i1>>()
    di::bind<i1, impl1>.in(di::shared)  |        ==
);                                      |        injector.create<shared_ptr<i1>>()
                                        | );
```
```cpp
Singleton scope (shared between threads)| Test
----------------------------------------|-----------------------------------------
auto injector = di::make_injector(      | assert(injector.create<shared_ptr<i1>>()
   di::bind<i1, impl1>.in(di::singleton)|        ==
);                                      |        injector.create<shared_ptr<i1>>()
                                        | );
```
```cpp
Session scope                           | Test
----------------------------------------|-----------------------------------------
auto my_session = []{};                 | assert(nullptr == injector.create<shared_ptr<i1>>());
                                        |
auto injector = di::make_injector(      | injector.call(di::session_entry(my_session));
    di::bind<i1, impl1>.in(             |
        di::session(my_session)         | assert(injector.create<shared_ptr<i1>>()
    )                                   |        ==
);                                      |        injector.create<shared_ptr<i1>>()
                                        | );
                                        |
                                        | injector.call(di::session_exit(my_session));
                                        |
                                        | assert(nullptr == injector.create<shared_ptr<i1>>());
```
```cpp
External scope                          | Test
----------------------------------------|-----------------------------------------
auto l = 42l;                           | assert(42 == injector.create<int>()); // external has priority
auto b = false;                         | assert(injector.create<shared_ptr<i1>>()
                                        |        ==
auto injector = di::make_injector(      |        injector.create<shared_ptr<i1>>()
   di::bind<int, int_<41>>              | );
 , di::bind<int>.to(42)                 | assert(l == injector.create<long&>());
 , di::bind<i1>.to(make_shared<impl>()) | assert(&l == &injector.create<long&>());
 , di::bind<long>.to(ref(l))            | assert(87 == injector.create<short>());
 , di::bind<short>.to([]{return 87;})   | {
 , di::bind<i2>.to(                     | auto object = injector.create<shared_ptr<i2>>();
     [&](const auto& injector)          | assert(nullptr == object);
        -> shared_ptr<i2> {             | }
            if (b) {                    | {
              return injector.template  | b = true;
                create<                 | auto object = injector.create<shared_ptr<i2>>();
                  shared_ptr<impl2>>(); | assert(dynamic_cast<impl2*>(object.get()));
            }                           | }
            return nullptr;             |
     }                                  |
   )                                    |
);                                      |
```
```cpp
Custom scope                            | Test
----------------------------------------|-----------------------------------------
struct custom_scope {                   | assert(injector.create<shared_ptr<i1>>()
  static constexpr                      |        !=
      auto priority = false;            |        injector.create<shared_ptr<i1>>()
                                        | );
  template<class TExpected, class>      |
  struct scope {                        |
    template<class T, class TProvider>  |
    auto create(const TProvider& pr) {  |
      return                            |
        shared_ptr<TExpected>{pr.get()};|
    }                                   |
  };                                    |
};                                      |
                                        |
auto injector = di::make_injector(      |
  di::bind<i1, impl1>.in(custom_scope{})|
);                                      |
```
| Type/Scope | unique | shared | singleton | session | external |
|------------|--------|--------|-----------|---------|----------|
| T | ✔ | - | - | - | ✔ |
| T& | - | - | - | - | ✔ |
| const T& | ✔ (temporary) | - | - | - | ✔ |
| T* | ✔ | - | - | - | ✔ |
| const T* | ✔ | - | - | - | ✔ |
| T&& | ✔ | - | - | - | - |
| unique\_ptr<T> | ✔ | - | - | - | ✔ |
| shared\_ptr<T> | ✔ | ✔ | ✔ | ✔ | ✔ |
| weak\_ptr<T> | - | ✔ | ✔ | ✔ | - |

*

<a id="modules"></a>
> **[Modules](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/concepts/modules.html)** | [Examples](https://github.com/krzysztof-jusiak/di/blob/cpp14/example/modules.cpp)
```cpp
Module                                  | Test
----------------------------------------|-----------------------------------------
struct c {                              | auto object = injector.create<unique_ptr<c>>();
    c(unique_ptr<i1> i1                 | assert(dynamic_cast<impl1*>(object->i1.get()));
    , unique_ptr<i2> i2                 | assert(dynamic_cast<impl2*>(object->i2.get()));
    , int i) : i1(move(i1))             | assert(42 == object->i);
             , i2(move(i2)), i(i)       |
    { }                                 | auto up1 = injector.create<unique_ptr<i1>>();
                                        | assert(dynamic_cast<impl1*>(up1.get()));
    unique_ptr<i1> i1;                  |
    unique_ptr<i2> i2;                  | auto up2 = injector.create<unique_ptr<i2>>();
    int i = 0;                          | assert(dynamic_cast<impl2*>(up2.get()));
};                                      |
                                        |
struct module1 {                        |
    auto configure() const noexcept {   |
        return di::make_injector(       |
            di::bind<i1, impl1>         |
          , di::bind<int>.to(42)        |
        );                              |
    }                                   |
};                                      |
                                        |
struct module2 {                        |
    auto configure() const noexcept {   |
        return di::make_injector(       |
            di::bind<i2, impl2>         |
        );                              |
    };                                  |
};                                      |
                                        |
auto injector = di::make_injector(      |
    module1{}, module2{}                |
);                                      |
```
```cpp
Exposed type module                     | Test
----------------------------------------|-----------------------------------------
struct c {                              | auto object = injector.create<c>();
    c(shared_ptr<i1> i1                 | assert(dynamic_cast<impl1*>(object.i1.get()));
    , shared_ptr<i2> i2                 | assert(dynamic_cast<impl2*>(object.i2.get()));
    , int i) : i1(i1), i2(i2), i(i)     | assert(42 == object.i);
    { }                                 |
                                        | // injector.create<unique_ptr<i1>>() // compile error
    shared_ptr<i1> i1;                  | // injector.create<unique_ptr<i2>>() // compile error
    shared_ptr<i2> i2;                  |
    int i = 0;                          |
};                                      |
                                        |
struct module {                         |
    di::injector<c> configure()         |
    const noexcept;                     |
                                        |
    int i = 0;                          |
};                                      |
                                        |
di::injector<c> // expose c             |
module::configure() const noexcept {    |
    return di::make_injector(           |
        di::bind<i1, impl1>             |
      , di::bind<i2, impl2>             |
      , di::bind<int>.to(i)             |
    );                                  |
}                                       |
                                        |
auto injector = di::make_injector(      |
    module{42}                          |
);                                      |
```
```cpp
Exposed many types module               | Test
----------------------------------------|-----------------------------------------
struct module {                         | auto up1 = injector.create<unique_ptr<i1>>();
    di::injector<i1, i2> configure()    | assert(dynamic_cast<impl1*>(up1.get()));
    const noexcept;                     |
                                        | auto up2 = injector.create<unique_ptr<i2>>();
    int i = 0;                          | assert(dynamic_cast<impl2*>(up2.get()));
};                                      |
                                        |
di::injector<i1, i2> // expose i1, i2   |
module::configure() const noexcept {    |
    return di::make_injector(           |
        di::bind<i1, impl1>             |
      , di::bind<i2, impl2>             |
    );                                  |
}                                       |
                                        |
auto injector = di::make_injector(      |
    module{}                            |
);                                      |
```
```cpp
Exposed type module with annotation     | Test
----------------------------------------|-----------------------------------------
auto my = []{};                         | auto object = injector.create<unique_ptr<c>>();
                                        | assert(dynamic_cast<impl1*>(object->up.get()));
struct c {                              |
    BOOST_DI_INJECT(c                   |
      , (named = my) unique_ptr<i1> up) |
      : up(up)                          |
    { }                                 |
                                        |
    unique_ptr<i1> up;                  |
};                                      |
                                        |
di::injector<i1> module =               |
    di::make_injector(                  |
        di::bind<i1, impl1>             |
    );                                  |
                                        |
auto injector = di::make_injector(      |
    di::bind<i1>.named(my).to(module)   |
);                                      |
```

*

<a id="providers"></a>
> **[Providers](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/concepts/providers.html)** | [Examples](https://github.com/krzysztof-jusiak/di/blob/cpp14/example/custom_provider.cpp)
* [heap](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/concepts/providers/heap.html)
* [stack\_over\_heap (default)](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/concepts/providers/stack_over_heap_default.html)
```cpp
Heap no throw provider                  | Test
----------------------------------------|-----------------------------------------
class heap_no_throw {                   | // per injector policy
public:                                 | auto injector = di::make_injector<my_provider>();
  template<                             | assert(0 == injector.create<int>());
    class // interface                  |
  , class T // implementation           | // global policy
  , class TInit // direct()/uniform{}   | #define BOOST_DI_CFG my_provider
  , class TMemory // heap/stack         | auto injector = di::make_injector();
  , class... TArgs>                     | assert(0 == injector.create<int>());
  auto get(const TInit&                 |
         , const TMemory&               |
         , TArgs&&... args)             |
  const noexcept {                      |
      return new (nothrow)              |
        T{forward<TArgs>(args)...};     |
  }                                     |
};                                      |
                                        |
class my_provider : public di::config { |
public:                                 |
    auto provider() const noexcept {    |
        return heap_no_throw{};         |
    }                                   |
};                                      |
```

*

<a id="policies"></a>
> **[Policies](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/concepts/policies.html)** | [Examples](https://github.com/krzysztof-jusiak/di/blob/cpp14/example/types_dumper.cpp) | [More examples](https://github.com/krzysztof-jusiak/di/blob/cpp14/example/custom_policy.cpp)
```cpp
Define policies configuration           | Test
(dump types)                            |
----------------------------------------|-----------------------------------------
class print_types_policy                | // per injector policy
    : public di::config {               | auto injector = di::make_injector<print_types_policy>();
public:                                 | injector.create<int>(); // output: int
  auto policies() const noexcept {      |
    return di::make_policies(           | // global policy
      [](auto type){                    | #define BOOST_DI_CFG my_policy
         using T = decltype(type);      | auto injector = di::make_injector();
         using arg = typename T::type;  | injector.create<int>(); // output: int
         cout << typeid(arg).name()     |
              << endl;                  |
      }                                 |
    );                                  |
  }                                     |
};                                      |
```
```cpp
Define policies configuration           | Test
(dump types extended)                   |
----------------------------------------|-----------------------------------------
class print_types_info_policy           | // per injector policy
    : public di::config {               | auto injector = di::make_injector<print_types_info_policy>(
public:                                 |     di::bind<i1, impl1>
  auto policies() const noexcept {      | );
    return di::make_policies(           |
      [](auto type                      | injector.create<unique_ptr<i1>>();
       , auto dep                       |
       , auto... ctor) {                | // output:
         using T = decltype(type);      |     0 // ctor_size of impl1
         using arg = typename T::type;  |     unique_ptr<i1> // ctor arg
         using arg_name =               |     di::no_name // ctor arg name
            typename T::name;           |     di::deduce // scope
         using D = decltype(dep);       |     i1 // expected
         using scope =                  |     impl1 // given
            typename D::scope;          |     no_name // dependency
         using expected =               |
            typename D::expected;       |
         using given =                  | // global policy
            typename D::given;          | #define BOOST_DI_CFG my_policy
         using name =                   | auto injector = di::make_injector(
            typename D::name;           |     di::bind<i1, impl1>
         auto ctor_s = sizeof...(ctor); | );
                                        |
         cout << ctor_s                 | injector.create<unique_ptr<i1>>();
              << endl                   |
              << typeid(arg).name()     | // output:
              << endl                   |     0 // ctor_size of impl1
              << typeid(arg_name).name()|     unique_ptr<i1> // cotr arg
              << endl                   |     di::no_name // ctor arg name
              << typeid(scope).name()   |     di::deduce // scope
              << endl                   |     i1 // expected
              << typeid(expected).name()|     impl1 // given
              << endl                   |     no_name // dependency
              << typeid(given).name()   |
              << endl                   |
              << typeid(name).name()    |
              << endl;                  |
         ;                              |
      }                                 |
    );                                  |
  }                                     |
};                                      |
|
```
```cpp
`constructible` policy                  | Test
----------------------------------------|-----------------------------------------
#include <boost/di/                     | // global policy
    policies/constructible.hpp>         | #define BOOST_DI_CFG all_must_be_bound_unless_int
                                        | assert(0 == di::make_injector().create<int>());
class all_must_be_bound_unless_int      |
    : public di::config {               | // di::make_injector().create<double>(); // compile error
public:                                 | assert(42.0 == make_injector(
  auto policies() const noexcept {      |                    di::bind<double>.to(42.0)
    using namespace di::policies;       |                ).create<double>()
    using namespace                     | );
        di::policies::operators;        |
                                        |
    return di::make_policies(           |
      constructible(                    |
        is_same<_, int>{} ||            |
        is_bound<_>{})                  |
    );                                  |
  }                                     |
};                                      |
    |
```

*

<a id="run_time_performance"></a>
> **[Run-time performance (-O2)](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/performance.html)**
* Environment
    * x86\_64 Intel(R) Core(TM) i7-4770 CPU @ 3.40GHz GenuineIntel GNU/Linux
    * clang++3.4 -O2 / `gdb -batch -ex 'file ./a.out' -ex 'disassemble main'`
```cpp
Create type wihtout bindings            | Asm x86-64 (same as `return 0`)
----------------------------------------|-----------------------------------------
int main() {                            | xor %eax,%eax
    auto injector = di::make_injector();| retq
    return injector.create<int>();      |
}                                       |
```
```cpp
Create type with bound instance         | Asm x86-64 (same as `return 42`)
----------------------------------------|-----------------------------------------
int main() {                            | mov $0x2a,%eax
    auto injector = di::make_injector(  | retq
        di::bind<int>.to(42)            |
    );                                  |
                                        |
    return injector.create<int>();      |
}                                       |
```
```cpp
Create named type                       | Asm x86-64 (same as `return 42`)
----------------------------------------|-----------------------------------------
auto my_int = []{};                     | mov $0x2a,%eax
                                        | retq
struct c {                              |
    BOOST_DI_INJECT(c                   |
        , (named = my_int) int i)       |
        : i(i)                          |
    { }                                 |
                                        |
    int i = 0;                          |
};                                      |
                                        |
int main() {                            |
  auto injector = di::make_injector(    |
    di::bind<int>.named(my_int).to(42)  |
  );                                    |
                                        |
  return injector.create<c>().i;        |
}                                       |
```
```cpp
Create bound interface                  | Asm x86-64 (same as `make_unique`)
----------------------------------------|-----------------------------------------
int main() {                            | push   %rax
    auto injector = di::make_injector(  | mov    $0x8,%edi
        di::bind<i1, impl1>             | callq  0x4007b0 <_Znwm@plt>
    );                                  | movq   $0x400a30,(%rax)
                                        | mov    $0x8,%esi
    auto ptr = injector.create<         | mov    %rax,%rdi
        unique_ptr<i1>                  | callq  0x400960 <_ZdlPvm>
    >();                                | mov    $0x1,%eax
                                        | pop    %rdx
    return ptr.get() != nullptr;        | retq
}                                       |
```
```cpp
Create bound interface via module       | Asm x86-64 (same as `make_unique`)
----------------------------------------|-----------------------------------------
struct module {                         | push   %rax
	auto configure() const noexcept {   | mov    $0x8,%edi
		return di::make_injector(       | callq  0x4007b0 <_Znwm@plt>
			di::bind<i1, impl1>         | movq   $0x400a10,(%rax)
		);                              | mov    $0x8,%esi
	}                                   | mov    %rax,%rdi
};                                      | callq  0x400960 <_ZdlPvm>
                                        | mov    $0x1,%eax
int main() {                            | pop    %rdx
	auto injector = di::make_injector(  | retq
        module{}                        |
    );                                  |
                                        |
	auto ptr = injector.create<         |
        unique_ptr<i1>                  |
    >();                                |
                                        |
	return ptr != nullptr;              |
}                                       |
```
```cpp
Create bound interface via exposed      | Asm x86-64
module                                  | cost = virtual call due to type erasure
----------------------------------------|----------------------------------------------------------------------
struct module {                         | push   %rbp                               mov    (%rax),%ecx
	di::injector<i1> configure() const {| push   %rbx                               lea    -0x1(%rcx),%edx
		return di::make_injector(       | sub    $0x38,%rsp                         mov    %edx,(%rax)
			di::bind<i1, impl1>         | lea    0x10(%rsp),%rdi                    cmp    $0x1,%ecx
		);                              | lea    0x8(%rsp),%rsi                     jne    0x400bcd <main+173>
	}                                   | callq  0x400bf0 <_ZN5boost2di7exposed>    mov    (%rbx),%rax
};                                      | mov    0x18(%rsp),%rdi                    mov    %rbx,%rdi
                                        | mov    (%rdi),%rax                        callq  *0x10(%rax)
int main() {                            | lea    0x30(%rsp),%rsi                    lea    0xc(%rbx),%rax
	auto injector = di::make_injector(  | callq  *0x10(%rax)                        mov    $0x0,%ecx
        module{}                        | test   %rax,%rax                          test   %rcx,%rcx
    );                                  | setne  %bpl                               je     0x400bb8 <main+152>
                                        | je     0x400b57 <main+55>                 mov    $0xffffffff,%ecx
	auto ptr = injector.create<         | mov    (%rax),%rcx                        lock   xadd %ecx,(%rax)
        unique_ptr<i1>                  | mov    %rax,%rdi                          mov    %ecx,0x30(%rsp)
    >();                                | callq  *0x8(%rcx)                         mov    0x30(%rsp),%ecx
                                        | mov    0x20(%rsp),%rbx                    jmp    0x400bbf <main+159>
	return ptr != nullptr;              | test   %rbx,%rbx                          mov    (%rax),%ecx
}                                       | je     0x400bcd <main+173>                lea    -0x1(%rcx),%edx
                                        | lea    0x8(%rbx),%rax                     mov    %edx,(%rax)
                                        | mov    $0x0,%ecx                          cmp    $0x1,%ecx
                                        | test   %rcx,%rcx                          jne    0x400bcd <main+173>
                                        | je     0x400b82 <main+98>                 mov    (%rbx),%rax
                                        | mov    $0xffffffff,%ecx                   mov    %rbx,%rdi
                                        | lock   xadd %ecx,(%rax)                   callq  *0x18(%rax)
                                        | mov    %ecx,0x30(%rsp)                    movzbl %bpl,%eax
                                        | mov    0x30(%rsp),%ecx                    add    $0x38,%rsp
                                        | jmp    0x400b89 <main+105>                pop    %rbx
                                        |                                           pop    %rbp
                                        | -->                                       retq
```

*

<a id="compile_time_performance"></a>
> **[Compile-time performance](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/performance.html)** | [Example](https://github.com/krzysztof-jusiak/di/blob/cpp14/test/pt/di.cpp)
* Environment
    * x86\_64 Intel(R) Core(TM) i7-4770 CPU @ 3.40GHz GenuineIntel GNU/Linux
    * clang++3.4 -O2
```cpp
Boost.DI header                         | Time [s]
----------------------------------------|-----------------------------------------
#include <boost/di.hpp>                 | 0.165
int main() { }                          |
```
```cpp
Legend:
    ctor    = raw constructor: c(int i, double d);
    inject  = inject constructor: BOOST_DI_INJECT(c, int i, double d);
    all     = all types exposed from module: auto configure();
    exposed = one type exposed from module: di::injector<c> configure();
```
[![small complexity](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/small_complexity.png)](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/small_complexity.png)
```cpp
* 4248897537 instances created
* 132 different types
* 10 modules
```
---
[![medium complexity](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/medium_complexity.png)](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/medium_complexity.png)
```cpp
* 1862039751439806464 instances created
* 200 different types
* 10 modules
```
---
[![big complexity](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/big_complexity.png)](https://raw.githubusercontent.com/krzysztof-jusiak/di/cpp14/doc/images/big_complexity.png)
```cpp
* 5874638529236910091 instances created
* 310 different types
* 100 different interfaces
* 10 modules
```

*

<a id="diagnostic_messages"></a>
> **[Diagnostic messages](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/diagnostic_messages.html)**
```cpp
Create interface without bound          | Error message
implementation                          |
----------------------------------------|[clang]--------------------------------------
auto injector = di::make_injector();    | warning: 'create' is deprecated: creatable constraint not satisfied
injector.create<i*>();                  |     injector.create<i*>();
                                        |              ^
                                        | note: 'create<i *, 0>' has been explicitly marked deprecated here
                                        |     T create() const {
                                        |       ^
                                        | error: inline function 'boost::di::abstract_type<i>::is_not_bound::error' is not defined
                                        |     error(_ = "type not bound, did you forget to add: 'di::bind<interface, implementation>'?")
                                        |     ^
                                        | note: used here
                                        |     constraint_not_satisfied{}.error();
                                        |
                                        |[gcc]----------------------------------------
                                        | error: inline function ‘constexpr T* boost::di::abstract_type<T>::is_not_bound::error(boost::di::_) const [with T = i]’ used but never defined
                                        |      error(_ = "type not bound, did you forget to add: 'di::bind<interface, implementation>'?")
                                        |      ^
                                        | error: call to ‘boost::di::core::injector<boost::di::config>::create<i*, 0>’ declared with attribute error: creatable constraint not satisfied
                                        |      injector.create<i*>();
```
```cpp
Ambiguous binding                       | Error message
----------------------------------------|-----------------------------------------
auto injector = di::make_injector(      | error: base class 'pair<int, no_name>'
    di::bind<int>.to(42)                | specified more than once as a direct
  , di::bind<int>.to(87)                | base class
);                                      |
                                        |
injector.create<int>();                 |
```
```cpp
Create not bound object with all bound  | Error message
policy                                  |
----------------------------------------|-----------------------------------------
class all_bound : public di::config {   | error: static_assert failed
public:                                 | "Type T is not allowed"
  auto policies() const noexcept {      |
    return di::make_policies(           |
      constructible(is_bound<_>{})      |
    );                                  |
  }                                     |
};                                      |
                                        |
auto injector =                         |
    di::make_injector<all_bound>();     |
                                        |
injector.create<int>();                 |
```
```cpp
Wrong annotation                        | Error message
(NAMED instead of named)                |
----------------------------------------|-----------------------------------------
auto name = []{};                       | error: use of undeclared identifier
                                        | 'named'
struct c {                              |
    BOOST_DI_INJECT(c                   |
        , (NAMED = name) int) { }       |
};                                      |
                                        |
di::make_injector().create<c>();        |
```

*

<a id="configuration"></a>
> **[Configuration](http://krzysztof-jusiak.github.io/di/cpp14/boost/libs/di/doc/html/di/overview.html#di.overview.configuration)**
```cpp
Macro                                   | Description
----------------------------------------|-----------------------------------------
BOOST_DI_CFG_CTOR_LIMIT_SIZE            | Limits number of allowed consturctor
                                        | parameters [0-10, default=10]
----------------------------------------|-----------------------------------------
BOOST_DI_CFG                            | Global configuration allows to customize
                                        | provider and policies
----------------------------------------|-----------------------------------------
BOOST_DI_INJECTOR                       | Named used internally by Boost.DI
                                        | to define constructor traits
                                        | [default=boost_di_injector__]
```

**Similar libraries**
* C++ | [dicpp](https://bitbucket.org/cheez/dicpp) | [Fruit](https://github.com/google/fruit)
* Java | [Dagger 2](https://github.com/google/dagger) | [Guice](https://github.com/google/guice)
* C# | [Ninject](http://www.ninject.org)

---

**Disclaimer** Boost.DI is not an official Boost library.

