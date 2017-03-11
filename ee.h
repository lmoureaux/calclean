#ifndef CALCLEAN_EE
#define CALCLEAN_EE

/**
 * @file
 * @brief  Header for EE filters
 * @author Louis Moureaux
 * @date   2017
 */

#include "calofilter.h"

namespace calo {

class ee_filter : public filter
{
public:
  ee_filter() {}

  bool operator() (const tower_ref &tower) const { return tower.isee(); }
};

/// An instance of @ref ee_filter.
/**
 * @relates ee_filter
 */
const ee_filter ee;

} // namespace calo

#endif // CALCLEAN_EE
