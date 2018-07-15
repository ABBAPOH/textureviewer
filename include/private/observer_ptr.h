// <experimental/memory> -*- C++ -*-

// Copyright (C) 2015-2018 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file experimental/memory
 *  This is a TS C++ Library header.
 */

//
// N4336 Working Draft, C++ Extensions for Library Fundamentals, Version 2
//

#ifndef OBSERVER_PTR_H
#define OBSERVER_PTR_H

#include <memory>
#include <type_traits>
#include <utility>

namespace Private
{
  template <typename _Tp>
    class observer_ptr
    {
    public:
      // publish our template parameter and variations thereof
      using element_type = _Tp;
      using __pointer = std::add_pointer_t<_Tp>;            // exposition-only
      using __reference = std::add_lvalue_reference_t<_Tp>; // exposition-only

      // 3.2.2, observer_ptr constructors
      // default c’tor
      constexpr observer_ptr() noexcept
      : __t()
      { }

      // pointer-accepting c’tors
      constexpr observer_ptr(std::nullptr_t) noexcept
      : __t()
      { }

      constexpr explicit observer_ptr(__pointer __p) noexcept
      : __t(__p)
      { }

      // copying c’tors (in addition to compiler-generated copy c’tor)
      template <typename _Up,
                typename = typename std::enable_if<
                  std::is_convertible<typename std::add_pointer<_Up>::type, __pointer
                  >::value
                >::type>
      constexpr observer_ptr(observer_ptr<_Up> __p) noexcept
      : __t(__p.get())
      {
      }

      // 3.2.3, observer_ptr observers
      constexpr __pointer
      get() const noexcept
      {
        return __t;
      }

      constexpr __reference
      operator*() const
      {
        return *get();
      }

      constexpr __pointer
      operator->() const noexcept
      {
        return get();
      }

      constexpr explicit operator bool() const noexcept
      {
        return get() != nullptr;
      }

      // 3.2.4, observer_ptr conversions
      constexpr explicit operator __pointer() const noexcept
      {
        return get();
      }

      // 3.2.5, observer_ptr modifiers
      constexpr __pointer
      release() noexcept
      {
        __pointer __tmp = get();
        reset();
        return __tmp;
      }

      constexpr void
      reset(__pointer __p = nullptr) noexcept
      {
        __t = __p;
      }

      constexpr void
      swap(observer_ptr& __p) noexcept
      {
        std::swap(__t, __p.__t);
      }

    private:
      __pointer __t;
    }; // observer_ptr<>

  template<typename _Tp>
    void
    swap(observer_ptr<_Tp>& __p1, observer_ptr<_Tp>& __p2) noexcept
    {
      __p1.swap(__p2);
    }

  template<typename _Tp>
    observer_ptr<_Tp>
    make_observer(_Tp* __p) noexcept
    {
      return observer_ptr<_Tp>(__p);
    }

  template<typename _Tp, typename _Up>
    bool
    operator==(observer_ptr<_Tp> __p1, observer_ptr<_Up> __p2)
    {
      return __p1.get() == __p2.get();
    }

  template<typename _Tp, typename _Up>
    bool
    operator!=(observer_ptr<_Tp> __p1, observer_ptr<_Up> __p2)
    {
    return !(__p1 == __p2);
    }

  template<typename _Tp>
    bool
    operator==(observer_ptr<_Tp> __p, std::nullptr_t) noexcept
    {
      return !__p;
    }

  template<typename _Tp>
    bool
    operator==(std::nullptr_t, observer_ptr<_Tp> __p) noexcept
    {
      return !__p;
    }

  template<typename _Tp>
    bool
    operator!=(observer_ptr<_Tp> __p, std::nullptr_t) noexcept
    {
      return bool(__p);
    }

  template<typename _Tp>
    bool
    operator!=(std::nullptr_t, observer_ptr<_Tp> __p) noexcept
    {
      return bool(__p);
    }

  template<typename _Tp, typename _Up>
    bool
    operator<(observer_ptr<_Tp> __p1, observer_ptr<_Up> __p2)
    {
      return std::less<typename std::common_type<typename std::add_pointer<_Tp>::type,
                                            typename std::add_pointer<_Up>::type
                                            >::type
                       >{}(__p1.get(), __p2.get());
    }

  template<typename _Tp, typename _Up>
    bool
    operator>(observer_ptr<_Tp> __p1, observer_ptr<_Up> __p2)
    {
      return __p2 < __p1;
    }

  template<typename _Tp, typename _Up>
    bool
    operator<=(observer_ptr<_Tp> __p1, observer_ptr<_Up> __p2)
    {
      return !(__p2 < __p1);
    }

  template<typename _Tp, typename _Up>
    bool
    operator>=(observer_ptr<_Tp> __p1, observer_ptr<_Up> __p2)
    {
      return !(__p1 < __p2);
    }

} // namespace Private

namespace std {

template <typename _Tp>
  struct hash<Private::observer_ptr<_Tp>>
  {
    using result_type = size_t;
    using argument_type = Private::observer_ptr<_Tp>;

    size_t
    operator()(const Private::observer_ptr<_Tp>& __t) const
    noexcept(noexcept(hash<typename add_pointer<_Tp>::type> {}(__t.get())))
    {
      return hash<typename add_pointer<_Tp>::type> {}(__t.get());
    }
  };
} // namespace std

#endif // OBSERVER_PTR_H
