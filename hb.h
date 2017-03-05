#ifndef CALCLEAN_HB
#define CALCLEAN_HB

/**
 * @file
 * @brief  Header for HB filters
 * @author Louis Moureaux
 * @date   2017
 */

#include "calofilter.h"
#include "logic.h"

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

class goodhb_filter : public filter
{
  static const int ETA_DIVS = 34;
  static const int PHI_DIVS = 360 / 5;

  float _energies[ETA_DIVS];
  std::vector<int> _hotcells[ETA_DIVS];

public:
  explicit inline goodhb_filter();
  explicit goodhb_filter(float energies[ETA_DIVS],
                         std::vector<int> hotcells[ETA_DIVS]);

  bool operator() (const tower_ref &tower) const;
};

/// Creates a filter with the default parameters.
goodhb_filter::goodhb_filter()
{
  /// @cond
#include "hb_default_params.cpp"
  /// @endcond
  for (int ieta = 0; ieta < ETA_DIVS; ++ieta) {
    _energies[ieta] = energies[ieta];
    _hotcells[ieta] = hotcells[ieta];
  }
}

/// An instance of @ref goodhb_filter using the default parameters.
/**
 * @relates goodhb_filter
 */
const goodhb_filter goodhb;

/// An filter for bad HB cells.
/**
 * This is the opposite of @ref goodhb.
 *
 * @relates goodhb_filter
 */
const not_filter badhb = not_filter(&goodhb);

} // namespace calo

#endif // CALCLEAN_HB
