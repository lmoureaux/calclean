#include "hb.h"

/**
 * @file
 * @brief  Source for HB filters
 * @author Louis Moureaux
 * @date   2017
 */

#include <cmath>

namespace calo {

/**
 * @defgroup HB HCAL Barrel
 * @brief Filters for HCAL Barrel towers.
 *
 * This module contains various filters related to the HCAL Barrel. The most
 * basic is @ref hb_filter "hb", which returns all HB towers. The
 * @ref coldhb_filter "coldhb" filter is designed to remove hot cells; and
 * @ref goodhb_filter "goodhb" defines energy thresholds.
 */

/**
 * @class hb_filter calclean/hb.h
 * @brief Iterate over all HB towers.
 *
 * One can use this filter to iterate over all towers from the HCAL Barrel.
 *
 * @ingroup HB
 */

} // namespace calo
