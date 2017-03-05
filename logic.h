#ifndef CALCLEAN_LOGIC
#define CALCLEAN_LOGIC

/**
 * @file
 * @brief  Header file for filters implementing logic operations
 * @author Louis Moureaux
 * @date   2017
 */

#include "calofilter.h"

namespace calo {

namespace detail {
  template<typename T>
  T *copy(const T &object)
  {
    return new T(object);
  }
} // namespace detail

class and_filter : public filter
{
  const filter *_lhs;
  const filter *_rhs;
public:
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

class or_filter : public filter
{
  const filter *_lhs, *_rhs;
public:
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

class not_filter : public filter
{
  const filter *_arg;
public:
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
