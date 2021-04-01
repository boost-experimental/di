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
  explicit impl2(i3& i, std::string str)
  {
    assert(i.f() == 1234);
    assert(str == "text");
  }
  int bar() const override { return 99; }
};



struct impl1 : i1 {
  explicit impl1(std::unique_ptr<i2> sp2) { assert(sp2->bar() == 99); }
  int foo() const override { return 42; }
};



struct i1_b {
  virtual ~i1_b() noexcept = default;
  virtual int foo_b() const = 0;
};

struct impl1b : i1, i1_b {
  explicit impl1b(std::unique_ptr<i2> sp2) { assert(sp2->bar() == 99); }
  int foo() const override { return 42; }
  int foo_b() const override { return 1042; }
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
  example(std::shared_ptr<i1> sp, int i, module_example& me) {
    assert(dynamic_cast<impl1*>(sp.get()));
    assert(sp->foo() == 42);
    assert(i == 87);
    assert(me.get() == 2 * 100);
  }
};

//auto ct_module = [] { return di::make_injector(di::bind<i4>().to<impl4>()); };
di::extension::runtime_injector rt_module() { return di::make_injector(di::bind<i4>().to<impl4>()); }

int main() {
  // clang-format off
  namespace di = boost::di;

  /*<<create runtime injector>>*/
  di::extension::runtime_injector injector{};

  /*<<install bindings>>*/
  injector.install(
    di::bind<i1>().to<impl1>(),
    di::bind<i1, i1_b>().to<impl1b>().in(boost::di::singleton),
        di::bind<i3>().to<impl3>(), // todo remove this when integrating with below
        di::bind<int>().to(87), // todo remove this as well...
        di::bind<std::string>().to("text"), // todo remove this as well...
    di::bind<i2, i2_xxx>().to<impl2>()
  );

  // Create one by itself - to get injected into constructors
  std::shared_ptr<i1_b> i1_b_ptr = injector.create<std::shared_ptr<i1_b>>();
  // std::cout << "i1_b_ptr is @" << i1_b_ptr << std::endl;


  {
    // Try out list first - which means give us only what has been created already
    typedef std::list<std::shared_ptr<i1>> i1_list_t;
    i1_list_t i1_list = injector.create<i1_list_t>();
    assert(1 == i1_list.size());

    int i1_b_count {0};
    for(auto & x : i1_list)
    {
      if(std::dynamic_pointer_cast<i1_b>(x))
      {
        ++i1_b_count;
        assert( &(*x) == &(* reinterpret_cast<i1*>(i1_b_ptr.get()) ) );
      }
    }
    assert(1 == i1_b_count);
  }


  {
     // Try out vector - which means construct all of them
     typedef std::vector<std::shared_ptr<i1>> i1_vec_t;
     i1_vec_t i1_vec = injector.create<i1_vec_t>();
     assert(2 == i1_vec.size());

     int i1_b_count {0};
     for(auto & x : i1_vec)
     {
        if(std::dynamic_pointer_cast<i1_b>(x))
        {
           ++i1_b_count;
           assert( &(*x) == &(* reinterpret_cast<i1*>(i1_b_ptr.get()) ) );
        }
     }
    assert(1 == i1_b_count);

  }


#if 0
  typedef std::vector<std::shared_ptr<i1>> i1_vec_t;
//  typedef std::list<std::shared_ptr<i1>> i1_vec_t; // TODO, omg this is horribly broken! lol!
  i1_vec_t i1_vec = injector.create<i1_vec_t>();
  std::cout << "i1_vec size " << i1_vec.size() << std::endl;
  for(auto & x : i1_vec)
  {
    std::cout << "   @" << x << " -> " << x->foo() << std::endl;
    if(std::dynamic_pointer_cast<i1_b>(x))
      std::cout << "   ^^ this is a i1_b type" << std::endl;
  }

  std::shared_ptr<i1> i1_by_self = injector.create<std::shared_ptr<i1>>();
  std::cout << "i1 by self is @" << i1_by_self << std::endl;


  std::shared_ptr<i3> i3_ptr = injector.create<std::shared_ptr<i3>>();
  std::cout << "i3 " << i3_ptr->f() << std::endl;
#endif

#if 0
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

// injector.dump_data_bindings();

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
#endif
  // clang-format on
}

