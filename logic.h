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
 */

namespace calo {

namespace detail {
  /// Returns a pointer to a copy of the object passed in argument.
  template<typename T>
  T *copy(const T &object)
  {
    return new T(object);
  }
} // namespace detail

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

template<typename T, typename U>
inline and_filter operator&& (const T &lhs, const U &rhs)
{
  return and_filter(detail::copy(lhs), detail::copy(rhs));
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

template<typename T, typename U>
inline or_filter operator|| (const T &lhs, const U &rhs)
{
  return or_filter(detail::copy(lhs), detail::copy(rhs));
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

template<typename T>
inline calo::not_filter operator! (const T &arg)
{
  return calo::not_filter(detail::copy(arg));
}

} // namespace calo

#endif // CALCLEAN_LOGIC
