#include "ee.h"

/**
 * @file
 * @brief  Source for EE filters
 * @author Louis Moureaux
 * @date   2017
 */

#include <cmath>

namespace calo {

/**
 * @defgroup EE ECAL Endcap
 * @brief Filters for ECAL Endcap towers.
 *
 * This module contains various filters related to the ECAL Endcap. The most
 * basic is @ref ee_filter "ee", which returns all EE towers. The
 * @ref coldee_filter "coldee" filter is designed to remove hot cells; and
 * @ref goodee_filter "goodee" defines energy thresholds.
 */

/**
 * @class ee_filter calclean/ee.h
 * @brief Iterate over all EE towers.
 *
 * One can use this filter to iterate over all towers from the ECAL Endcap.
 *
 * @ingroup EE
 */

} // namespace calo
