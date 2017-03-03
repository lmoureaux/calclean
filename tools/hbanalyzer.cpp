/**
 * @file
 * @brief  Program to determine HB thresholds
 * @author Louis Moureaux
 * @date   2017
 */

#include <cstring> // strerror
#include <cstdlib>
#include <iostream>
#include <string>

#include <Math/RootFinder.h>
#include <TH1D.h>
#include <TFile.h>
#include <TMath.h>

#include "calofilter.h"
#include "hb.h"

/// The number of bins in @f$\eta@f$.
const int ETA_DIVS = 34;
/// The number of bins in @f$\phi@f$.
const int PHI_DIVS = 360 / 5;

void printusage(const char *progname);

/// Holds the configuration.
class config
{
  double _pvalue;
  int _numhot;
public:
  explicit config(double pvalue, int numhot) :
    _pvalue(pvalue), _numhot(numhot) {}
  /// Returns the target pvalue.
  double pvalue() const { return _pvalue; }
  /// Returns the target number of hot cells for the given eta bin.
  double numhot() const { return _numhot; }
};

/// Holds the results.
class results
{
  double _energies[ETA_DIVS];
  std::vector<int> _hotcells[ETA_DIVS];
public:
  explicit results(const config &c, calo::towerset &tset);
  /// Returns the threshold energy for the given @c ieta.
  double energy(int ieta) const { return _energies[ieta + ETA_DIVS / 2]; }
  /// Returns the hot cells for the given @c ieta.
  std::vector<int> hotcells(int ieta) const
  { return _hotcells[ieta + ETA_DIVS / 2]; }
};

/// Entry point for the program.
int main(int argc, char *argv[])
{
  // Do we have enough arguments?
  if (argc < 2) {
    std::cerr << "Error: Not enough parameters" << std::endl;
    printusage(argv[0]);
    std::exit(1);
  } else if (argc > 3) {
    std::cerr << "Error: Too many parameters" << std::endl;
    printusage(argv[0]);
    std::exit(1);
  }

  // Read data
  TFile *in = new TFile(argv[1], "READ");
  if (!in->IsOpen()) {
    std::cerr << "Error: could not open '" << argv[1] << "': "
              << std::strerror(in->GetErrno()) << std::endl;
    std::exit(1);
  }
  in->cd();

  // Get the pvalue from the command line
  double pvalue = 0.01;
  if (argc >= 3) {
    const char *start = argv[2];
    char *end;
    pvalue = std::strtod(start, &end);
    if (start == end || (long) std::strlen(argv[2]) != end - start) {
      std::cerr << "Error: cannot interpret '" << argv[2] << "' as a number"
                << std::endl;
      std::exit(1);
    }
  }
  config c(pvalue, 0);

  results *r;
  try {
    calo::towerset tset;
    r = new results(c, tset);
  } catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Error: Unexpected exception" << std::endl;
  }

  std::cout << "return {" << std::endl;
  for (int ieta = -ETA_DIVS / 2; ieta < ETA_DIVS / 2; ++ieta) {
    std::vector<int> ignored = r->hotcells(ieta);
    std::cout << "  [" << ieta << "] = { energy = " << r->energy(ieta) << ", ";
    for (unsigned iphi = 0; iphi < ignored.size(); ++iphi) {
      std::cout << ignored[iphi] << ", ";
    }
    std::cout << "}," << std::endl;
  }
  std::cout << "}" << std::endl;

  return 0;
}

////////////////////////////////////////////////////////////////////////////////

/// Prints a short help message.
void printusage(const char *progname)
{
  std::cerr << "Usage: " << progname << " <data.root> [pvalue]" << std::endl;
  std::cerr << "Where:" << std::endl;
  std::cerr << "  <data.root> contains a valid calofilter tree" << std::endl;
  std::cerr << "  pvalue is the target pvalue (0.01)" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

/// Small struct for HB hits
struct hit
{
  float eta, phi, hadenergy;

  /// All hits
  static std::vector<hit> all;
};
std::vector<hit> hit::all;

TH1D *make_histogram(int ieta, double energy);

struct stats
{
  double pvalue;
  int maxphi;
  explicit stats(const TH1D *hits, const std::vector<int> &ignore);
};

struct cost_function
{
  double pvalue;
  int numhot;
  int ieta;

  std::vector<int> getignored(double energy) const;
  double operator() (double energy) const;
};

results::results(const config &c, calo::towerset &tset)
{
  // Load all hits into hit::all (for optimization)
  hit::all.reserve(100000);
  calo::hb_filter hb;
  for (unsigned long entry = 0; entry < tset.entries(); ++entry) {
    tset.getentry(entry);

    calo::towerset::iterator end = tset.end();
    for (calo::towerset::iterator it = tset.begin(&hb); it != end; ++it) {
      hit h;
      h.eta = it->eta();
      h.phi = it->phi();
      h.hadenergy = it->hadenergy();
      hit::all.push_back(h);
    }
  }

  // Optimizer
  ROOT::Math::RootFinder rootfinder;
  rootfinder.SetMethod(ROOT::Math::RootFinder::kGSL_BRENT);

  // For each eta
  for (int ieta = -ETA_DIVS / 2; ieta < ETA_DIVS / 2; ++ieta) {
    cost_function cost;
    cost.ieta = ieta;
    cost.pvalue = c.pvalue();
    cost.numhot = c.numhot();

    int status = rootfinder.Solve(cost, 0, 10, 500, 1e-4, 1e-4);
    if (status != 1) {
      std::cerr << "Error: Optimization failled for ieta=" << ieta << std::endl;
      std::exit(2);
    }
    double energy = std::ceil((rootfinder.Root() + 1e-3) * 100) / 100;

    std::vector<int> ignored = cost.getignored(energy);
    for (unsigned i = 0; i < ignored.size(); ++i) {
      ignored[i] -= PHI_DIVS / 2;
    }

    _energies[ieta + ETA_DIVS / 2] = energy;
    _hotcells[ieta + ETA_DIVS / 2] = ignored;
  }
}

TH1D *make_histogram(int ieta, double energy)
{
  TH1D *hits = new TH1D("hits", "hits", PHI_DIVS, -M_PI, M_PI);

  const std::vector<hit>::iterator end = hit::all.end();
  for (std::vector<hit>::iterator it = hit::all.begin(); it != end; ++it) {
    if (it->hadenergy > energy && ieta == std::floor(it->eta / 0.085)) {
      hits->Fill(it->phi);
    }
  }

  return hits;
}

stats::stats(const TH1D *hits, const std::vector<int> &ignore)
{
  int max = 0;

  double sum = 0;
  double sumsq = 0;

  for (int i = 1; i <= hits->GetNbinsX(); ++i) {
    if (std::find(ignore.begin(), ignore.end(), i) == ignore.end()) {
      double content = hits->GetBinContent(i);
      if (content > 0) {
        sum += content;
        sumsq += content * content;
        if (content > max) {
          max = content;
          maxphi = i;
        }
      }
    }
  }

  int numgood = PHI_DIVS - ignore.size();

  double mean = sum / numgood;
  double chi2 = sumsq / mean - numgood * mean;

  pvalue = TMath::Prob(chi2, numgood - 1);
}

std::vector<int> cost_function::getignored(double energy) const
{
  std::vector<int> ignore;

  TH1D *hits = make_histogram(ieta, energy);
  stats s(hits, ignore);

  while (s.pvalue < pvalue) {
    ignore.push_back(s.maxphi);
    s = stats(hits, ignore);
  }

  delete hits;

  return ignore;
}

double cost_function::operator() (double energy) const
{
  int ignored = getignored(energy).size();
  return ignored - numhot + (ignored > numhot ? energy : -energy);
}
