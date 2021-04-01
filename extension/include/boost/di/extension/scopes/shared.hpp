//
// Copyright (c) 2012-2020 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#if !defined(BOOST_DI_NOT_THREAD_SAFE)
#include <mutex>
#endif

#include "boost/di.hpp"

BOOST_DI_NAMESPACE_BEGIN
namespace extension {

namespace detail {
class shared {
 public:
  template <class, class T>
  class scope {
   public:
    scope() noexcept = default;

#if !defined(BOOST_DI_NOT_THREAD_SAFE)
    //<<lock mutex so that move will be synchronized>>
    explicit scope(scope&& other) noexcept : scope(std::move(other), std::lock_guard<std::mutex>(other.mutex_)) {}
    //<<synchronized move constructor>>
    scope(scope&& other, const std::lock_guard<std::mutex>&) noexcept : object_(std::move(other.object_)) {}
#endif

    template <class T_, class>
    using is_referable = typename wrappers::shared<shared, T>::template is_referable<T_>;

    template <class, class, class TProvider>
    static auto try_create(const TProvider& provider)
        -> decltype(wrappers::shared<shared, T>{std::shared_ptr<T>{provider.get()}});

    /**
     * `in(shared)` version
     */
    template <class, class, class TProvider>
    wrappers::shared<shared, T> create(const TProvider& provider) & {
      if (!object_) {
#if !defined(BOOST_DI_NOT_THREAD_SAFE)
        std::lock_guard<std::mutex> lock(mutex_);
        if (!object_)
#endif
          object_ = std::shared_ptr<T>{provider.get()};
      }
      return wrappers::shared<shared, T>{object_};
    }

    /**
     * Deduce scope version
     */
    template <class, class, class TProvider>
    wrappers::shared<shared, T> create(const TProvider& provider) && {
      auto& object = provider.cfg().template data<T>();
      if (!object) {
#if !defined(BOOST_DI_NOT_THREAD_SAFE)
        std::lock_guard<std::mutex> lock(mutex_);
        if (!object)
#endif
        {
          auto raw_ptr_from_get = provider.get();

          // Check one last time and use that instead so that the shared_ptr comes from the right place
          if(object)
          {
            return wrappers::shared<shared, T>{std::static_pointer_cast<T>(object)};
          }

          object = std::shared_ptr<T>{raw_ptr_from_get};
        }
      }
      return wrappers::shared<shared, T>{std::static_pointer_cast<T>(object)};
    }


   private:
#if !defined(BOOST_DI_NOT_THREAD_SAFE)
    std::mutex mutex_;
#endif
    std::shared_ptr<T> object_;  /// used by `in(shared)`, otherwise destroyed immediately
  };
};
}  // namespace detail

static constexpr detail::shared shared{};

class shared_config : public di::config {

 public:
  template <class T>
  struct scope_traits {
    using type = typename di::config::scope_traits<T>::type;
  };

  template <class T>
  struct scope_traits<T&> {
    using type = detail::shared;
  };

  template <class T>
  struct scope_traits<std::shared_ptr<T>> {
    using type = detail::shared;
  };

  template <class T>
  struct scope_traits<boost::shared_ptr<T>> {
    using type = detail::shared;
  };

  template <class T>
  struct scope_traits<std::weak_ptr<T>> {
    using type = detail::shared;
  };

  template <class T>
  auto& data() {
    return data_[std::type_index(typeid(T))];
  }

 private:
  std::unordered_map<std::type_index, std::shared_ptr<void>> data_{};
};

}  // namespace extension
BOOST_DI_NAMESPACE_END
