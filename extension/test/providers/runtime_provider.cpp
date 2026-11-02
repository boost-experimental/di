//
// Copyright (c) 2012-2020 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "boost/di/extension/providers/runtime_provider.hpp"

#include <cassert>
#include <string>

namespace di = boost::di;

struct i1 {
  virtual ~i1() noexcept = default;
  virtual int foo() const = 0;
};

struct i2 {
  virtual ~i2() noexcept = default;
  virtual int bar() const = 0;
};

struct i3 {
  virtual ~i3() noexcept = default;
  virtual int f() const = 0;
};

struct i4 {
  virtual ~i4() noexcept = default;
  virtual int foo() const = 0;
};

struct impl3 : i3 {
  explicit impl3(const int& i) { assert(i == 87); }
  int f() const override { return 1234; }
};


struct i2_xxx {
  virtual ~i2_xxx() noexcept = default;
  virtual int other_bar() const { return 420; }
};

struct impl2 : i2, i2_xxx {
  explicit impl2(i3& i, std::string str) {
    assert(i.f() == 1234);
    assert(str == "text");
  }
  int bar() const override { return 99; }
};

struct impl1 : i1 {
  explicit impl1(std::unique_ptr<i2> sp2) { assert(sp2->bar() == 99); }
  int foo() const override { return 42; }
};

class ctor;
class dtor;

struct impl4 : i4 {
  template <class>
  static auto& calls() {
    static auto i = 0;
    return i;
  }

  impl4() { ++calls<ctor>(); }
  ~impl4() { ++calls<dtor>(); }

  int foo() const override { return 100; }
};

class module_example {
 public:
  explicit module_example(std::shared_ptr<i4> sp) : sp{sp} {}
  auto get() const { return sp->foo() * 2; }

 private:
  std::shared_ptr<i4> sp{};
};

struct example {
  example(std::shared_ptr<i1> sp, int i /*, module_example& me*/) {
    assert(dynamic_cast<impl1*>(sp.get()));
    assert(sp->foo() == 42);
    assert(i == 87);
//    assert(me.get() == 2 * 100);
  }
};

auto ct_module = [] { return di::make_injector(di::bind<i4>().to<impl4>()); };
di::extension::runtime_injector rt_module() { return di::make_injector(di::bind<i4>().to<impl4>()); }

int main() {
  // clang-format off
  namespace di = boost::di;

  /*<<create runtime injector>>*/
  di::extension::runtime_injector injector{};

  /*<<install bindings>>*/
  injector.install(
    di::bind<i1>().to<impl1>(),
    di::bind<i2, i2_xxx>().to<impl2>()
  );

  /*<<more bindings>>*/
  injector.install(di::bind<int>().to(87));

  /*<<injector bindings>>*/
  auto component = di::make_injector(
    di::bind<i3>().to<impl3>(),
    di::bind<std::string>().to("text")
  );
  injector.install(component);

  /*<<module bindings>>*/
  injector.install(ct_module());

  /*<<create example>>*/
  injector.create<example>();

  // scoped injector
  {
    impl4::calls<ctor>() = {};
    impl4::calls<dtor>() = {};

    di::extension::runtime_injector injector{};
    injector.install(rt_module());
    auto m = di::create<module_example>(injector);

    assert(m.get() == 2 * 100);
    assert(impl4::calls<ctor>() == 1);
    assert(impl4::calls<dtor>() == 0);
  }
  assert(impl4::calls<ctor>() == 1);
  assert(impl4::calls<dtor>() == 1);

  // clang-format on
}
