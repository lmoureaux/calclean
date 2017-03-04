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

/**
 * @class goodhb_filter calclean/hb.h
 * @brief Iterate over good HB towers.
 *
 * This filter can be used to iterate over "good" towers in HB. The filter uses
 * a different threshold energy for each @f$\eta@f$ bin. The list of hot cells
 * is also given in @f$\eta@f$ bins.
 *
 * The default thresholds and hot cell lists were defined by requiring that the
 * @f$\phi@f$ distribution be uniform in every @f$\eta@f$ bin, with a P-value
 * higher than 0.01. The number of cells to remove was defined manually in each
 * bin, and the @ref hbanalyzer "helper tools" were used to build the list.
 *
 * @ingroup HB
 */

/// Constructs a filter with the given parameters.
/**
 * The parameters should have been built by the
 * @ref formatting-cppcode "cppcode" formatting option of the HB helper tools.
 */
goodhb_filter::goodhb_filter(float energies[ETA_DIVS],
                             std::vector<int> hotcells[ETA_DIVS])
{
  for (int ieta = 0; ieta < ETA_DIVS; ++ieta) {
    _energies[ieta] = energies[ieta];
    _hotcells[ieta] = hotcells[ieta];
  }
}

bool goodhb_filter::operator() (const tower_ref &tower) const
{
  int ieta = std::floor(tower.eta() / 0.085) + ETA_DIVS / 2;
  // Energy cut
  if (tower.hadenergy() < _energies[ieta]) {
    return false;
  }
  // Hot cells
  int iphi = std::floor(tower.phi() * 2 * M_PI / PHI_DIVS);
  const std::vector<int>::const_iterator end = _hotcells[ieta].end();
  for (std::vector<int>::const_iterator it = _hotcells[ieta].begin();
       it != end; ++it) {
    if (iphi == *it) {
      return false;
    }
  }
  return true;
}

} // namespace calo
