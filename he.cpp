#include "he.h"

/**
 * @file
 * @brief  Source for HE filters
 * @author Louis Moureaux
 * @date   2017
 */

#include <cmath>

namespace calo {

/**
 * @defgroup HE HCAL Endcap
 * @brief Filters for HCAL Endcap towers.
 *
 * This module contains various filters related to the HCAL Endcap. The most
 * basic is @ref he_filter "he", which returns all HE towers. The
 * @ref coldhe_filter "coldhe" filter is designed to remove hot cells; and
 * @ref goodhe_filter "goodhe" defines energy thresholds.
 */

/**
 * @class he_filter calclean/he.h
 * @brief Iterate over all HE towers.
 *
 * One can use this filter to iterate over all towers from the HCAL Endcap.
 *
 * @ingroup HE
 */

} // namespace calo
