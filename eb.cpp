#include "eb.h"

/**
 * @file
 * @brief  Source for EB filters
 * @author Louis Moureaux
 * @date   2017
 */

#include <cmath>

namespace calo {

/**
 * @defgroup EB ECAL Barrel
 * @brief Filters for ECAL Barrel towers.
 *
 * This module contains various filters related to the ECAL Barrel. The most
 * basic is @ref eb_filter "eb", which returns all EB towers. The
 * @ref coldeb_filter "coldeb" filter is designed to remove hot cells; and
 * @ref goodeb_filter "goodeb" defines energy thresholds.
 */

/**
 * @class eb_filter calclean/eb.h
 * @brief Iterate over all EB towers.
 *
 * One can use this filter to iterate over all towers from the ECAL Barrel.
 *
 * @ingroup EB
 */

/**
 * @class coldeb_filter calclean/eb.h
 * @brief Iterate over "cold" EB towers.
 *
 * Some towers in the EB are anomalously active (they are "hot"). This filters
 * allows one to iterate over all but hot towers.
 *
 * ### Technical details
 *
 * Hot towers are handled by defining logical coordinates for towers as
 * @f$ i_\eta \equiv \left\lfloor \frac{\eta}{0.085} \right\rfloor @f$ and
 * @f$ i_\phi \equiv \left\lfloor \frac{36 \phi}{\pi} \right\rfloor @f$. This
 * corresponds to the groups of 5 by 5 crystals that @c CaloTowers use.
 *
 * The filter uses a list of hot cells' logical coordinates to remove them. The
 * default list is:
 *
 *  @f$ i_\eta @f$ | @f$ i_\phi @f$
 * :--------------:|:--------------:
 *       -16       |      -36
 *       -16       |      -35
 *       -15       |      -35
 *       -11       |      -35
 *       -18       |       35
 *       -17       |       35
 *       -16       |       35
 *       -15       |       35
 *       -17       |      -11
 *       -10       |       -7
 *        -9       |        0
 *         8       |       -8
 *         2       |       11
 *         0       |       11
 *        -6       |       24
 *       -18       |       31
 *        11       |       11
 *        13       |       12
 *        14       |       12
 *        14       |       11
 *        15       |       11
 *        16       |       11
 *
 * @ingroup EB
 */

/// Constructs a filter for the given hot cells
/**
 * The first two arguments are vectors containing the position of hot cells. The
 * first should contain the @f$ i_\eta @f$ logical coordinates, and the second
 * the corresponding values of @f$ i_\phi @f$. Both vectors must have the same
 * size.
 */
coldeb_filter::coldeb_filter(const std::vector<int> &hotcells_eta,
                             const std::vector<int> &hotcells_phi) :
  _hotcells_eta(hotcells_eta),
  _hotcells_phi(hotcells_phi)
{
  assert(_hotcells_eta.size() == _hotcells_phi.size());
}

bool coldeb_filter::operator() (const tower_ref &tower) const
{
  if (tower.iseb()) {
    // Remove hot cells
    int ieta = std::floor(tower.eta() / 0.085);
    int iphi = std::floor(tower.phi() / M_PI * 36);
    for (unsigned i = 0; i < _hotcells_eta.size(); ++i) {
      if (ieta == _hotcells_eta[i] && iphi == _hotcells_phi[i]) {
        return false;
      }
    }
    return true;
  } else {
    return false;
  }
}

/**
 * @class goodeb_filter
 * @brief A filter that cleans up EB
 *
 * This filter implements hot tower and noise removal for the electromagnetic
 * barrel. A set of default cuts is hard-coded, but can be customized easily.
 *
 * The recommended use of this class is through its static instance @ref goodeb.
 *
 * ### How the cut works
 *
 * Since towers can correspond to groups of several EB crystals, the good
 * variable is the mean energy per crystal, @f$E/N@f$. Several thresholds are
 * used depending on the number of crystals; a tower above them is considered
 * good.
 *
 * The default thresholds are:
 *
 * Number of crystals | Threshold [GeV/crystal]
 * :-----------------:|:-----------------------:
 *          1         |          0.37
 *          2         |          0.28
 *          3         |          0.25
 *         >3         |          0.22
 *
 * ### Hot towers
 *
 * @c goodeb_filter uses @ref coldeb_filter internally. See the documentation of
 * this class for more information.
 *
 * @ingroup EB
 */

/// Constructs a filter with the given thresholds
/**
 * The first two arguments are vectors containing the position of hot cells; see
 * @ref coldeb_filter for more information.
 *
 * The @c thresholds vector should contain the thresholds for
 * @f$E_\mathrm{em}/N@f$. The first
 * element will be used when @f$N = 1@f$; the second when @f$N = 2@f$, and so
 * on. When no threshold is found in the vector (ie it is too short), the last
 * element is used.
 */
goodeb_filter::goodeb_filter(const std::vector<int> &hotcells_eta,
                             const std::vector<int> &hotcells_phi,
                             const std::vector<float> &thresholds) :
  _cold(hotcells_eta, hotcells_phi),
  _thresholds(thresholds)
{}

bool goodeb_filter::operator() (const tower_ref &tower) const
{
  if (tower.iseb() && _cold(tower)) {
    // Filter energy
    int crystals = tower.ebcount();
    if ((unsigned) crystals < _thresholds.size()) {
      return tower.emenergy() > _thresholds[crystals - 1] * crystals;
    } else {
      return tower.emenergy() > _thresholds.back() * crystals;
    }
  } else {
    return false;
  }
}

} // namespace calo
