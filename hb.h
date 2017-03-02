#ifndef CALCLEAN_HB
#define CALCLEAN_HB

/**
 * @file
 * @brief  Header for HB filters
 * @author Louis Moureaux
 * @date   2017
 */

#include "calofilter.h"

namespace calo {

class hb_filter : public filter
{
public:
  hb_filter() {}

  bool operator() (const tower_ref &tower) const { return tower.ishb(); }
};

/// An instance of @ref hb_filter.
/**
 * @relates hb_filter
 */
const hb_filter hb;

} // namespace calo

#endif // CALCLEAN_HB
