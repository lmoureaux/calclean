#ifndef CALCLEAN_HE
#define CALCLEAN_HE

/**
 * @file
 * @brief  Header for HE filters
 * @author Louis Moureaux
 * @date   2017
 */

#include "calofilter.h"

namespace calo {

class he_filter : public filter
{
public:
  he_filter() {}

  bool operator() (const tower_ref &tower) const { return tower.ishe(); }
};

/// An instance of @ref he_filter.
/**
 * @relates he_filter
 */
const he_filter he;

} // namespace calo

#endif // CALCLEAN_HE
