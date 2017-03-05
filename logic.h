#ifndef CALCLEAN_LOGIC
#define CALCLEAN_LOGIC

/**
 * @file
 * @brief  Header file for filters implementing logic operations
 * @author Louis Moureaux
 * @date   2017
 */

#include "calofilter.h"

/**
 * @defgroup logic Logic
 * @brief Filters implementing boolean logic
 *
 * This module contains filters that implement basic logical operations. In
 * addition, it overloads logical operators to work on filters.
 *
 * @warning Chaining logical operators will lead to memory leaks.
 */

namespace calo {

/// A filter that implements a logical AND between two filters
/**
 * @warning This class doesn't delete its arguments upon destruction. This
 *          behaviour easily leads to memory leaks.
 * @ingroup logic
 */
class and_filter : public filter
{
  const filter *_lhs;
  const filter *_rhs;
public:
  /// Creates a filter that returns @c true when both @c lhs and @c rhs are
  /// @c true
  and_filter(const filter *lhs, const filter *rhs) : _lhs(lhs), _rhs(rhs) {}

  bool operator() (const tower_ref &tower) const;
};

bool and_filter::operator() (const tower_ref &tower) const
{
  const filter &lhs = *_lhs;
  const filter &rhs = *_rhs;
  const tower_ref t = tower; // ROOT's pseudo-C++ parser
  return lhs(t) && rhs(t);
}

/// A filter that implements a logical OR between two filters
/**
 * @warning This class doesn't delete its arguments upon destruction. This
 *          behaviour easily leads to memory leaks.
 * @ingroup logic
 */
class or_filter : public filter
{
  const filter *_lhs, *_rhs;
public:
  /// Creates a filter that returns @c true when at least one of @c lhs and
  /// @c rhs is @c true
  or_filter(const filter *lhs, const filter *rhs) : _lhs(lhs), _rhs(rhs) {}

  bool operator() (const tower_ref &tower) const;
};

bool or_filter::operator() (const tower_ref &tower) const
{
  const filter &lhs = *_lhs;
  const filter &rhs = *_rhs;
  const tower_ref t = tower; // ROOT's pseudo-C++ parser
  return lhs(t) || rhs(t);
}

/// A filter that negates another (logical NOT)
/**
 * @warning This class doesn't delete its arguments upon destruction. This
 *          behaviour easily leads to memory leaks.
 * @ingroup logic
 */
class not_filter : public filter
{
  const filter *_arg;
public:
  /// Creates a filter that returns @c true when @c arg is @c false
  explicit not_filter(const filter *arg) : _arg(arg) {}

  bool operator() (const tower_ref &tower) const;
};

bool not_filter::operator() (const tower_ref &tower) const
{
  const filter &arg = *_arg;
  const tower_ref t = tower; // ROOT's pseudo-C++ parser
  return !arg(t);
}

////////////////////////////////////////////////////////////////////////////////

// ROOT's pseudo-C++ parser
#ifndef __CINT__

namespace detail {
  /// Returns a pointer to a copy of the object passed in argument.
  template<typename T>
  T *copy(const T &object)
  {
    return new T(object);
  }
} // namespace detail

/// Logical AND between two filters
/**
 * This operator makes a copy of @c lhs and @c rhs and builds an @ref and_filter
 * from them.
 *
 * @note Due to a bug in CINT, this operator is disabled when using ROOT in
 *       interpreted mode.
 *
 * @tparam T The type of the left-hand-side argument. It must be
 *           copy-constructible.
 * @tparam U The type of the right-hand-side argument. It must be
 *           copy-constructible.
 * @ingroup logic
 */
template<typename T, typename U>
inline calo::and_filter operator&& (const T &lhs, const U &rhs)
{
  return and_filter(detail::copy(lhs), detail::copy(rhs));
}

/// Logical NOT on a filter
/**
 * This operator makes a copy of @c arg and builds a @ref not_filter from it.
 *
 * @note Due to a bug in CINT, this operator is disabled when using ROOT in
 *       interpreted mode.
 *
 * @tparam T The type of the argument. It must be copy-constructible.
 * @ingroup logic
 */
template<typename T>
inline calo::not_filter operator! (const T &arg)
{
  return calo::not_filter(detail::copy(arg));
}

/// Logical OR between two filters
/**
 * This operator makes a copy of @c lhs and @c rhs and builds an @ref or_filter
 * from them.
 *
 * @note Due to a bug in CINT, this operator is disabled when using ROOT in
 *       interpreted mode.
 *
 * @tparam T The type of the left-hand-side argument. It must be
 *           copy-constructible.
 * @tparam U The type of the right-hand-side argument. It must be
 *           copy-constructible.
 * @ingroup logic
 */
template<typename T, typename U>
inline calo::or_filter operator|| (const T &lhs, const U &rhs)
{
  return or_filter(detail::copy(lhs), detail::copy(rhs));
}

#endif // __CINT__

} // namespace calo

#endif // CALCLEAN_LOGIC
