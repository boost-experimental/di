//
// Copyright (c) 2012-2020 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <list>
#include <utility>

#include "boost/di.hpp"
#include "boost/di/extension/scopes/shared.hpp"

// Additions
#include "boost/di/extension/providers/runtime_provider_additions.h"
#include "boost/di/extension/providers/runtime_provider_additions_any_of.h"
#include "boost/di/extension/providers/runtime_provider_additions_di_core_array.h"
#include "boost/di/extension/providers/runtime_provider_additions_is_singleton.h"
#include "boost/di/extension/providers/runtime_provider_generator_saver.h"

static size_t unique_binding_id {0}; // TODO, is this kosher to put here? - Could just generate a UUID each time

// Move to another file once we know what we're doing here
class ibinding_entry {
public:
   virtual ~ibinding_entry() {}
   virtual size_t unique_id() const = 0;
   virtual void* get_it(const bool is_heap) = 0;
   virtual bool is_singleton() const = 0;
   virtual void for_each_type_id(std::function<void(const std::type_index)> fn) = 0;
   virtual size_t type_id_size() const = 0;
   virtual std::string binding_str() const = 0;
};


template <typename PROVIDER_T, typename BINDING_T, typename T, typename TYPEID_ARRAY>
class my_binding_entry : public ibinding_entry {
public:
 my_binding_entry
    ( const bool singleton_context
    , PROVIDER_T* provider
    , BINDING_T binding
    , const size_t id
    , const TYPEID_ARRAY & type_id_array
    , std::unordered_map<size_t, size_t> & mapping
    ) : m_singleton_context(singleton_context)
      , m_provider(provider)
      , m_binding(binding)
      , m_id(id)
      , m_type_id_array(type_id_array)
      , m_mapping(mapping)
  {}

  size_t unique_id() const override { return m_id; }

  void* get_it(const bool is_heap ) override
  {
    // Under new scheme just always make it - we have to find "shared" versions at a different level
    auto res = m_provider->template make<T>(m_binding);

    if(is_heap && m_singleton_context)
    {
      m_mapping[reinterpret_cast<std::uintptr_t>(res)] = m_id;
    }

    return res;
  }

  bool is_singleton() const override { return m_singleton_context; }

  void for_each_type_id(std::function<void(const std::type_index)> fn) override
  {
     for(const auto & type_id_entry : m_type_id_array)
     {
        fn(type_id_entry);
     }
  }

  size_t type_id_size() const override
  {
     return m_type_id_array.size();
  }

  std::string binding_str() const override
  {
      const std::string r = std::type_index(typeid(BINDING_T)).name();
      return r;
  }

private:
  bool m_singleton_context {false};
  PROVIDER_T* m_provider {nullptr};
  BINDING_T m_binding;
  size_t m_id {0};
  TYPEID_ARRAY m_type_id_array;
  std::unordered_map<size_t, size_t> & m_mapping;
};



BOOST_DI_NAMESPACE_BEGIN
namespace extension {

struct assert_error_policy {
  template <class T>
  void error() const {
    assert(false && "Type not bound!");
  }
};

template <class TErrorPolicy, class TScopeTraits>
class runtime_provider : public config {
  using bindings_t = std::unordered_map<std::type_index, std::vector<std::shared_ptr<ibinding_entry>>>;
  using data_t = std::unordered_map<std::type_index, std::list<std::shared_ptr<void>>>;
  // Map created addresses to unique ids of generator fns - so we can tell which generator functions don't need to be called again
  using mapping_t = std::unordered_map<size_t, size_t>;

  class abstract_provider : TErrorPolicy {
   public:
    explicit abstract_provider(const bindings_t &bindings, data_t & data, const mapping_t & mapping) : bindings_{bindings}, data_ptr_{&data}, mapping_(mapping) {}

    template <class...>
    struct is_creatable {
      static constexpr auto value = true;
    };


    // DWK - get di core array vector ...
    template <class T, class TInitialization, class TMemory, class... TArgs>
    typename std::enable_if<is_di_core_array<T>::value && !is_di_core_array_list<T>::value, T>::type*
    get(const TInitialization & , const TMemory & , TArgs &&... args) const
    {
      // Create container (vector<std::shared_ptr<>>)
      auto vec = get<T>(std::integral_constant < bool,
                            !std::is_abstract<T>::value && std::is_constructible<T, TArgs...>::value > {},
                        std::forward<TArgs>(args)...);

      typedef typename is_di_core_array<T>::subtype entry_t;
//      typedef typename is_di_core_array<T>::raw_ptr_t raw_ptr_t;
//      typedef typename is_di_core_array<T>::ptr_t ptr_t;

      // -------------------------------------------------------------------
//      if(create_all)
      {
        // TODO, make this optional to collect vs generate_all: std::list vs std::vector?
        // Force creation from all bindings of this type
        auto my_bindings_itr = bindings_.find(typeid(entry_t));
        if(my_bindings_itr != bindings_.end())
        {
          for(auto & generator_fn : my_bindings_itr->second)
          {
            // -------------------------------
            // Determine if we need to run this generator or if it has already been saved off previously (under right conditions: singleton + heap)
            bool already_run {false};
            for(const auto & m : mapping_)
            {
              if(m.second == generator_fn->unique_id())
              {
//                std::cout << "Skipping already created " << m.second << ", " << generator_fn->binding_str() << std::endl;
                already_run = true;
                break;
              }
            }
            if(already_run)
              continue;
            // -------------------------------

            void* generated_void_ptr = generator_fn->get_it(true);
            entry_t* generated_t_ptr = static_cast<entry_t*>(generated_void_ptr);

            if(generator_fn->type_id_size() > 0)
            {
              std::shared_ptr<entry_t> generated_t_shared_ptr(generated_t_ptr);
              std::shared_ptr<void> generated_void_shared_ptr(generated_t_shared_ptr);

              //          auto entry_t_type_index = std::type_index(typeid(entry_t));
              //          auto & data_list = (*data_ptr_)[entry_t_type_index];
              //          data_list.push_back(generated_void_shared_ptr);

              generator_fn->for_each_type_id([&](const std::type_index & ti)
                                             {
                                               auto & data_list = (*data_ptr_)[ti];
                                               data_list.push_back(generated_void_shared_ptr);
                                             });
            }

            //          auto xx = generator_fn->get_it(true); // always on heap this way .. prob?
//          auto xx_idx = std::type_index(typeid(entry_t));
//          auto & xyz = (*data_ptr_)[xx_idx];
//          auto xyz_shared = std::shared_ptr<entry_t>(reinterpret_cast<entry_t>(xx));
//          auto xyz_void_shared = std::shared_ptr<void>(xyz_shared);
//          xyz.push_back(xyz_void_shared);
          }
        }
      }
      // -------------------------------------------------------------------

      // Look in data_ and if nothing around then return empty vec
      const auto ti = std::type_index(typeid(entry_t));
      auto data_itr = data_ptr_->find(ti);

      if(data_itr == data_ptr_->end())
      {
        return vec;
      }
      // else

      //
      for(auto & ptr : data_itr->second)
      {
        std::shared_ptr<void> xx = ptr;
        std::shared_ptr<entry_t> my_ptr = std::static_pointer_cast<entry_t>(xx);
        vec->push_back( my_ptr );
      }

      return vec;
    }


    // DWK - get di core array list ...
    template <class T, class TInitialization, class TMemory, class... TArgs>
    typename std::enable_if<is_di_core_array_list<T>::value && !is_di_core_array<T>::value, T>::type*
    get(const TInitialization &, const TMemory &, TArgs &&... ) const
    {
      // Create container (vector<std::shared_ptr<>>)
// TODO, using this below causes us to create a list that already has entries in it
//      auto vec = get<T>(std::integral_constant < bool,
//                            !std::is_abstract<T>::value && std::is_constructible<T, TArgs...>::value > {},
//                        std::forward<TArgs>(args)...);

      auto vec = get<T>(std::integral_constant < bool,
                            !std::is_abstract<T>::value && std::is_constructible<T, TArgs...>::value > {}
                        );

//      typedef typename std::enable_if<is_di_core_array_list<T>::value && !is_di_core_array<T>::value, T>::type* res_t;
      typedef typename is_di_core_array_list<T>::subtype entry_t;
//      res_t* vec = new res_t();

//      typedef typename is_di_core_array<T>::raw_ptr_t raw_ptr_t;
//      typedef typename is_di_core_array<T>::ptr_t ptr_t;


#if 0
      // -------------------------------------------------------------------
      if(create_all)
      {
        // TODO, make this optional to collect vs generate_all: std::list vs std::vector?
        // Force creation from all bindings of this type
        auto my_bindings_itr = bindings_.find(typeid(entry_t));
        if(my_bindings_itr != bindings_.end())
        {
          for(auto & generator_fn : my_bindings_itr->second)
          {
            // -------------------------------
            // Determine if we need to run this generator or if it has already been saved off previously (under right conditions: singleton + heap)
            bool already_run {false};
            for(const auto & m : mapping_)
            {
              if(m.second == generator_fn->unique_id())
              {
                already_run = true;
                break;
              }
            }
            if(already_run)
              continue;
            // -------------------------------

            void* generated_void_ptr = generator_fn->get_it(true);
            entry_t* generated_t_ptr = static_cast<entry_t*>(generated_void_ptr);

            if(generator_fn->type_id_size() > 0)
            {
              std::shared_ptr<entry_t> generated_t_shared_ptr(generated_t_ptr);
              std::shared_ptr<void> generated_void_shared_ptr(generated_t_shared_ptr);

              //          auto entry_t_type_index = std::type_index(typeid(entry_t));
              //          auto & data_list = (*data_ptr_)[entry_t_type_index];
              //          data_list.push_back(generated_void_shared_ptr);

              generator_fn->for_each_type_id([&](const std::type_index & ti)
                                             {
                                               auto & data_list = (*data_ptr_)[ti];
                                               data_list.push_back(generated_void_shared_ptr);
                                             });
            }

            //          auto xx = generator_fn->get_it(true); // always on heap this way .. prob?
//          auto xx_idx = std::type_index(typeid(entry_t));
//          auto & xyz = (*data_ptr_)[xx_idx];
//          auto xyz_shared = std::shared_ptr<entry_t>(reinterpret_cast<entry_t>(xx));
//          auto xyz_void_shared = std::shared_ptr<void>(xyz_shared);
//          xyz.push_back(xyz_void_shared);
          }
        }
      }
      // -------------------------------------------------------------------
#endif
// DWK 12-17-2020
      // Look in data_ and if nothing around then return empty vec
      const auto ti = std::type_index(typeid(entry_t));
      auto data_itr = data_ptr_->find(ti);

      if(data_itr == data_ptr_->end())
      {
        return vec;
      }
      // else

      //
      for(auto & ptr : data_itr->second)
      {
        std::shared_ptr<void> xx = ptr;
        std::shared_ptr<entry_t> my_ptr = std::static_pointer_cast<entry_t>(xx);
        vec->push_back( my_ptr );
      }

      return vec;
    }




    template <class T, class TInitialization, class TMemory, class... TArgs>
    typename std::enable_if<!is_di_core_array<T>::value && !is_di_core_array_list<T>::value, T>::type*
    get(const TInitialization &, const TMemory &, TArgs &&... args) const
    {
      const bool is_heap = std::is_same<TMemory, boost::ext::di::v1_2_0::type_traits::heap>();

      auto it = bindings_.find(std::type_index(typeid(T)));
      if (it == bindings_.end())
      {
        return get<T>(std::integral_constant < bool,
                          !std::is_abstract<T>::value && std::is_constructible<T, TArgs...>::value > {},
                      std::forward<TArgs>(args)...);
      }
      assert(!it->second.empty());
      assert((*it->second.begin()) != nullptr);
      auto my_shared_ptr = (*it->second.begin());
      const std::string t_type_name_str = typeid(T).name();

      void* generated_void_ptr = my_shared_ptr->get_it(is_heap);
      T* generated_t_ptr = static_cast<T*>(generated_void_ptr);

      if(is_heap && my_shared_ptr->is_singleton())
      {
        std::shared_ptr<T> generated_t_shared_ptr(generated_t_ptr);
        std::shared_ptr<void> generated_void_shared_ptr(generated_t_shared_ptr);

        my_shared_ptr->for_each_type_id([&](const std::type_index & ti)
            {
              auto & data_list = (*data_ptr_)[ti];
              if(data_list.size() == 1 && nullptr == (*data_list.begin()))
                (*data_list.begin()) = generated_void_shared_ptr;
              else
                data_list.push_back(generated_void_shared_ptr);
            });
      }
      return generated_t_ptr; // Have to be careful not to use this in the shared.hpp
    }



   private:
    template <class T, class... TArgs>
    auto get(std::true_type, TArgs &&... args) const {
      return new T{std::forward<TArgs>(args)...};
    }

    template <class T, class... TArgs>
    auto get(std::false_type, TArgs &&...) const {
      this->template error<T>();
      return static_cast<T *>(nullptr);
    }

    const bindings_t &bindings_;
//    const data_t &data_;
    data_t* data_ptr_;
    const mapping_t & mapping_;
  };

 public:
  template <class T>
  using scope_traits = typename TScopeTraits::template scope_traits<T>;

  auto provider(...) { return abstract_provider{bindings_, data_, mapping_}; }
  auto &bindings() { return bindings_; }
  const auto &bindings() const { return bindings_; }

  template <class T>
  auto &data()
  {
    const auto t_idx = std::type_index(typeid(T));
    if(data_[t_idx].empty()) // auto-populate data_ with [t_idx]
      data_[t_idx].push_back(std::shared_ptr<void*>(nullptr));
    assert(data_[t_idx].size() > 0);
    return data_[std::type_index(typeid(T))].front();
  }

  // Used to populate data_ from functor for creating instance
  const auto& get_full_data() const
  {
    return data_;
  }

  const auto& get_full_bindings() const
  { return bindings_; }

  auto& get_full_mapping()
  { return mapping_; }

 private:
  bindings_t bindings_{};
  data_t data_{};
  mapping_t mapping_{};
};

namespace detail {
template <class TErrorPolicy, class TScopeTraits>
class injector : public core::injector<runtime_provider<TErrorPolicy, TScopeTraits>> {
  template <class...>
  struct any;

  template <class TParent, class TInjector>
  struct any<core::any_type_fwd<TParent>, TInjector> {
    using type = core::any_type<TParent, TInjector>;
  };

  template <class TParent, class TInjector>
  struct any<core::any_type_ref_fwd<TParent>, TInjector> {
    using type = core::any_type_ref<TParent, TInjector>;
  };

  template <class TParent, class TInjector>
  struct any<core::any_type_1st_fwd<TParent>, TInjector> {
    using type = core::any_type_1st<TParent, TInjector>;
  };

  template <class TParent, class TInjector>
  struct any<core::any_type_1st_ref_fwd<TParent>, TInjector> {
    using type = core::any_type_1st_ref<TParent, TInjector>;
  };

 public:
  injector() : core::injector<runtime_provider<TErrorPolicy, TScopeTraits>>{core::init{}} {}

  template <class T>
  /*non explicit*/ injector(const T &bindings) : injector() {
    install(bindings);
  }

//  template <class T, std::enable_if_t<!std::is_base_of<core::injector_base, T>::value, int> = 0>
//  void install(const T &binding) {
//    this->cfg().bindings()[std::type_index(typeid(typename T::expected))].push_back( [this, binding] {
//      return make<typename T::given>(binding);
//    });
//  }

#if 0
  // Debug
  void dump_data_bindings()
  {
    std::cout << "----------------------------------\n";
    std::cout << "dump data\n";
    const std::unordered_map<std::type_index, std::list<std::shared_ptr<void>>> & d = this->cfg().get_full_data();
    for(const auto & d_i : d)
    {
      std::cout << "  data: " << d_i.first.name() << " -> " << d_i.second.size() << "\n";
      for(const auto & v_i : d_i.second)
      {
        std::cout << "    " << v_i << "\n";
      }
    }
    std::cout << "----------------------------------\n";

    std::cout << "dump bindings\n";
    for(const auto & binding : this->cfg().get_full_bindings())
    {
      // bindings_t = std::unordered_map<std::type_index, std::list<std::shared_ptr<ibinding_entry>>>;
      std::cout << "binding " << binding.first.name() << " -> count " << binding.second.size() << std::endl;
      for(const auto & boundee : binding.second)
      {
        std::cout << "    " << (*boundee).my_address_of() << std::endl;
      }
    }

    std::cout << "----------------------------------\n";
  }
#endif

  int& gs_num()
  {
    static int gs_num {0};
    return gs_num;
  }


  template <class T, std::enable_if_t<!std::is_base_of<core::injector_base, T>::value, int> = 0>
  void install(const T &binding) {

    auto type_index_array = dependency_any_to_type_index_vec<T>();

//    std::cout << "type_index_array " << type_index_array.size() << std::endl;
//    for(const auto & ti : type_index_array)
//    {
//      std::cout << " --> " << ti.name() << std::endl;
//    }

    const bool is_singleton = dependency_is_singleton_scope<T>();

#if 0
//    std::cout << "Creating generator saver for " << typeid(T).name() << "\n";
    std::shared_ptr<generator_saver> gs_ptr = std::make_shared<generator_saver>(
        [ this
        , binding
        , my_is_singleton = dependency_is_singleton_scope<T>()
//        , my_data = &(this->cfg().get_full_data())
        , my_type_index_array = type_index_array
        ]
        {
//            this->cfg().bindings()[std::type_index(typeid(typename T::expected))].push_back( [this, binding] {
//              return make<typename T::given>(binding);
//            });
//          std::cout << "CALLING MAKE for type with " << my_type_index_array.size() << " bindings" << std::endl;
//          for(const auto & x : my_type_index_array)
//            std::cout << "  --> " << x.name() << std::endl;
//          // Make entry once
//          auto made_entity = make<typename T::given>(binding);
//          // Stuff entry pointer into all places in data_ that it can support based on type_index
//          dwk_add_to_data<T>(my_is_singleton, this->cfg().get_full_data(), my_type_index_array, made_entity);
//          // Return what we made
//          return made_entity;
        });
#endif

#if 0
// does NOT want to work
    std::shared_ptr<generator_saver> gs_ptr = std::make_shared<generator_saver>([this, binding] (){ return make<typename T::given>(binding); });

    for(const auto & id : type_index_array)
    {
      this->cfg().bindings()[id].push_back(
          [gs_ptr] () mutable
          {
             return gs_ptr->do_it();
          });
    }
#endif

#if 0
    typedef generator_saver<typename T::given> gs_t;
    std::shared_ptr<gs_t> gs_ptr = std::make_shared<gs_t>(++gs_num(), [this, binding] (){ return make<typename T::given>(binding); });

    for(const auto & id : type_index_array)
    {
      std::cout << "mapping (" << id.name() << ") to (" << typeid(typename T::given).name() << ")" << std::endl;

      this->cfg().bindings()[id].push_back(
//          [this, binding] () mutable
          [gs_ptr]
          {
//            generator_saver gs([this, binding] (){ return make<typename T::given>(binding); });
//            return gs.do_it();
            return gs_ptr->do_it();
          });
    }
#endif

#if 0
// WORKS
    for(const auto & id : type_index_array)
    {
      this->cfg().bindings()[id].push_back(
          [this, binding] ()
          {
              return make<typename T::given>(binding);
          });
    }
#endif


    // Let's try this
//    typedef std::function<void*()> fn_t;
//    std::shared_ptr<fn_t> ptr_fn(new fn_t([this, binding] ()
//                                          {
//                                            if(mt->ptr == nullptr)
//                                              mt->ptr = make<typename T::given>(binding);
//                                            return mt->ptr;
//                                          }));

    typedef my_binding_entry<injector<TErrorPolicy, TScopeTraits>, T, typename T::given, decltype(type_index_array)> my_thing_t;
    std::shared_ptr<ibinding_entry> my_thing_ptr = std::shared_ptr<my_thing_t>(new my_thing_t(is_singleton, this, binding, ++unique_binding_id, type_index_array, this->cfg().get_full_mapping()));

    for(const auto & id : type_index_array)
    {
      this->cfg().bindings()[id].push_back(
          my_thing_ptr );
    }

  }

  template <class T, std::enable_if_t<std::is_base_of<core::injector_base, T>::value, int> = 0>
  void install(const T &injector) {
    install(typename T::deps{}, injector, aux::identity<typename T::config>{});
  }

  template <class... TBindings>
  void install(const TBindings &... bindings) {
    (void)aux::swallow{0, (install(bindings), 0)...};
  }

 private:
  template <class TInjector, class TCfg, class TST>
  void install(aux::type_list<>, const TInjector &injector, aux::identity<runtime_provider<TCfg, TST>>) {
    for (const auto &b : injector.cfg().bindings()) {
      this->cfg().bindings()[b.first] = b.second;
    }
  }

  template <class... Ts, class TInjector, class T>
  void install(aux::type_list<Ts...>, const TInjector &injector, aux::identity<T>) {
    (void)aux::swallow{0, (install(static_cast<const Ts &>(injector)), 0)...};
  }

  template <class T, class TInit, class... Ts>
  auto make_impl(aux::pair<TInit, aux::type_list<Ts...>>) {
    return new T{typename any<Ts, injector>::type{*this}...};
  }
 public:

  template <class T, class TBinding>
  auto make(const TBinding &) -> decltype(make_impl<T>(typename ctor_traits<T>::type{})) {
    return make_impl<T>(typename ctor_traits<T>::type{});
  }

  template <class T, class TBinding>
  auto make(const TBinding &binding) -> decltype(new T{binding.object_})
  {
    return new T{binding.object_};
  }
};
}  // detail

using runtime_injector = detail::injector<assert_error_policy, di::extension::shared_config>;

}  // namespace extension
BOOST_DI_NAMESPACE_END
