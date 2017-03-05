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
 * This module contains filters that implement basic logical operations.
 *
 * @warning Classes in this module don't delete their arguments.
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

  inline bool operator() (const tower_ref &tower) const;
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

  inline bool operator() (const tower_ref &tower) const;
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

  inline bool operator() (const tower_ref &tower) const;
};

bool not_filter::operator() (const tower_ref &tower) const
{
  const filter &arg = *_arg;
  const tower_ref t = tower; // ROOT's pseudo-C++ parser
  return !arg(t);
}

} // namespace calo

#endif // CALCLEAN_LOGIC
