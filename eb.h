#ifndef CALCLEAN_EB
#define CALCLEAN_EB

/**
 * @file
 * @brief  Header for EB filters
 * @author Louis Moureaux
 * @date   2017
 */

#include "calofilter.h"

namespace calo {

class eb_filter : public filter
{
public:
  bool operator() (const tower_ref &tower) const { return tower.iseb(); }
};

/// An instance of @ref eb_filter.
/**
 * @relates eb_filter
 */
const eb_filter eb;

class coldeb_filter : public filter
{
  std::vector<int> _hotcells_eta;
  std::vector<int> _hotcells_phi;
public:
  explicit inline coldeb_filter();
  explicit coldeb_filter(const std::vector<int> &hotcells_eta,
                         const std::vector<int> &hotcells_phi);

  bool operator() (const tower_ref &tower) const;
};

// Needs to be before declaring the static goodeb because of Cint
/// Constructs a filter with the default hot towers list.
coldeb_filter::coldeb_filter()
{
  _hotcells_eta.push_back(-16); _hotcells_phi.push_back(-36);
  _hotcells_eta.push_back(-16); _hotcells_phi.push_back(-35);
  _hotcells_eta.push_back(-15); _hotcells_phi.push_back(-35);
  _hotcells_eta.push_back(-11); _hotcells_phi.push_back(-35);

  _hotcells_eta.push_back(-18); _hotcells_phi.push_back( 35);
  _hotcells_eta.push_back(-17); _hotcells_phi.push_back( 35);
  _hotcells_eta.push_back(-16); _hotcells_phi.push_back( 35);
  _hotcells_eta.push_back(-15); _hotcells_phi.push_back( 35);

  _hotcells_eta.push_back(-17); _hotcells_phi.push_back(-11);
  _hotcells_eta.push_back(-10); _hotcells_phi.push_back( -7);

  _hotcells_eta.push_back( -9); _hotcells_phi.push_back(  0);
  _hotcells_eta.push_back(  8); _hotcells_phi.push_back( -8);

  _hotcells_eta.push_back(  2); _hotcells_phi.push_back( 11);
  _hotcells_eta.push_back(  0); _hotcells_phi.push_back( 11);

  _hotcells_eta.push_back( -6); _hotcells_phi.push_back( 24);

  _hotcells_eta.push_back(-18); _hotcells_phi.push_back( 31);

  _hotcells_eta.push_back( 11); _hotcells_phi.push_back( 11);
  _hotcells_eta.push_back( 13); _hotcells_phi.push_back( 12);
  _hotcells_eta.push_back( 14); _hotcells_phi.push_back( 12);
  _hotcells_eta.push_back( 14); _hotcells_phi.push_back( 11);
  _hotcells_eta.push_back( 15); _hotcells_phi.push_back( 11);
  _hotcells_eta.push_back( 16); _hotcells_phi.push_back( 11);
}

/// An instance of @ref coldeb_filter using the default parameters.
/**
 * @relates coldeb_filter
 */
static const coldeb_filter coldeb;

class goodeb_filter : public filter
{
  coldeb_filter _cold;
  std::vector<float> _thresholds;
public:
  explicit inline goodeb_filter();
  explicit goodeb_filter(const std::vector<int> &hotcells_eta,
                         const std::vector<int> &hotcells_phi,
                         const std::vector<float> &thresholds);

  bool operator() (const tower_ref &tower) const;
};

// Needs to be before declaring the static goodeb because of Cint
/// Constructs a filter with default parameters
goodeb_filter::goodeb_filter()
{
  _thresholds.push_back(0.37);
  _thresholds.push_back(0.28);
  _thresholds.push_back(0.25);
  _thresholds.push_back(0.22);
}

/// An instance of @ref goodeb_filter using the default parameters.
/**
 * @relates goodeb_filter
 */
static const goodeb_filter goodeb;

} // namespace calo

#endif // CALCLEAN_EB