#include "calofilter.h"

#include <stdexcept>
#include <string>

#include <TBranch.h>
#include <TDirectory.h>
#include <TTree.h>

/**
 * @mainpage Quick Start Guide
 * @tableofcontents
 *
 * This library is a small framework to deal with hits (towers) in the CMS
 * calorimeters. The API is based on C++ iterators, a concept from the @c C++
 * standard template library (@c stl) that proved very successful. Iterators are
 * a kind of generalized pointers and are used to iterate over collections of
 * similar objects.
 *
 * @section Guide The Guide
 *
 * @subsection Import Importing the library
 *
 * The easiest way to import the library is by including the @c .cpp file:
 *
 * ~~~~{.cpp}
 * #include "calofilter.cpp"
 * ~~~~
 *
 * It is convenient to import the whole library into the global namespace:
 *
 * ~~~~{.cpp}
 * using namespace calo;
 * ~~~~
 *
 * Cleaner ways to get the same result are discussed @ref Compiled "below".
 *
 * @subsection Set Making a set
 *
 * The central class is @ref calo::towerset. It fetches information from a ROOT
 * tree and turns it into a nice form. If you have a file that contains the
 * @c CaloTree, you can declare a @c towerset as follows:
 *
 * ~~~~{.cpp}
 * TFile *file = new TFile("/path/to/your/file.root", "READ");
 * towerset tset(file);
 * ~~~~
 *
 * Notice that we didn't get the @c TTree from the file. The whole point of
 * @c towertset is to wrap the low-level tree into a higher-level interface.
 * This way, the tree contents can be changed at will without breaking the API.
 *
 * @subsection Iteration Iterating over events
 *
 * This is done in a fashion very similar to what @c TTree does:
 *
 * ~~~~{.cpp}
 * // For each event
 * unsigned long count = tset.entries();
 * for (unsigned long entry = 0; entry < count; ++entry) {
 *   tset.getentry(entry);
 *   // ...
 * }
 * ~~~~
 *
 * @subsection Hits Getting the hits
 *
 * You may want to loop over all hits:
 *
 * ~~~~{.cpp}
 * // For each tower
 * const towerset::iterator end = tset.end();
 * for (towerset::iterator it = tset.begin(); it != end; ++it) {
 *   // ...
 * }
 * ~~~~
 *
 * The @c it object will point to every hit in the event. The easiest way to get
 * information about a particular tower is to use the @c -> operator:
 *
 * ~~~~{.cpp}
 * bool iseb = it->iseb();
 * bool emenergy = it->emenergy();
 * ~~~~
 *
 * The full list of supported methods is available in the @ref calo::tower
 * documentation.
 *
 * It is also posible, albeit slower, to use @ref calo::tower objects directly:
 *
 * ~~~~{.cpp}
 * tower t = *it;
 * bool iseb = t.iseb();
 * bool emenergy = t.emenergy();
 * ~~~~
 *
 * @section Advanced Advanced topics
 *
 * @subsection Compiled Using a compiled version
 *
 * On Linux, the library can be compiled to a static library by using the
 * @c make command. If your analysis code is in a file named @c main.cpp and it
 * has a @c main() function, you can create an executable using the following
 * command:
 *
 * ~~~~
 * c++ `root-config --cflags` -O2 main.cpp libcalofilter.a -o analysis `root-config --libs`
 * ~~~~
 *
 * You will then have to include the header file only:
 *
 * ~~~~{.cpp}
 * #include "calofilter.h"
 * ~~~~
 *
 * Your code can then be run like any other program:
 *
 * ~~~~
 * ./analysis
 * ~~~~
 *
 * Since Cint sometimes happily accepts perfectly invalid code (just like it
 * rejects some valid constructs), trying to compile a Cint macro will likely
 * lead to a few compilation errors. Modern compilers, however, print very good
 * diagnostics, and might even suggest a fix.
 *
 * If you think that the command line above is too long, have a look at @c make.
 * It is a great tool for writing modular code.
 *
 * @subsection Cpp11 Simplified syntax with C++11
 *
 * The version of C++ published in 2011 has a simplified syntax for looping over
 * a collection:
 *
 * ~~~~{.cpp}
 * // For each tower
 * for (auto &tower : tset) {
 *   // ...
 * }
 * ~~~~
 *
 * Most modern compilers support this feature (you may have to use
 * @c -std=c++11). Use it if you can.
 */

namespace calo {

/// Prints a tower to a stream.
/**
 * @warning
 * This function is there for debugging purposes; the format of the output
 * should not be relied on.
 *
 * @relates tower
 */
std::ostream &operator<< (std::ostream &out, const tower &t)
{
  out << "calo::tower{"
      << " eta=" << t.eta()
      << ", phi=" << t.phi()
      << ", emenergy=" << t.emenergy()
      << ", hadenergy=" << t.hadenergy()
      << ", totalenergy=" << t.totalenergy()
      << " }";
  return out;
}

/// Constructs a towerset from the current directory
/**
 * A @c TTree named @c CaloTree is looked for in the current directory
 * (@c gDirectory). If it is found, it is used as per the @c TTree constructor.
 * Else, an exception is thrown (@c std::runtime_error). The @c towerset takes
 * ownership of the tree, and you shouldn't access it directly.
 */
towerset::towerset()
{
  TTree *tree = nullptr;
  gDirectory->GetObject("CaloTree", tree);
  if (tree == nullptr) {
    throw std::runtime_error("towerset::towerset: No TTree named \"CaloTree\" "
                             "found in the current directory");
  }
  _tree = tree;
  init_branches();
}

/// Constructs a towerset from the given directory
/**
 * A @c TTree named @c CaloTree is looked for in the given directory. If it is
 * found, it is used as per the @c TTree constructor. Else, an exception is
 * thrown (@c std::runtime_error). The @c towerset takes ownership of the tree,
 * and you shouldn't access it directly.
 *
 * This constructor is useful to read data directly from a ROOT file.
 */
towerset::towerset(TDirectory *dir)
{
  TTree *tree = nullptr;
  dir->GetObject("CaloTree", tree);
  if (tree == nullptr) {
    throw std::runtime_error("towerset::towerset: No TTree named \"CaloTree\" "
                             "found in the current directory");
  }
  _tree = tree;
  init_branches();
}

/// Constructs a towerset from the given @c TTree
/**
 * The tree is expected to contain the following branches:
 *
 *   - @c CaloSize: integer that gives the number of towers
 *   - <tt>CaloEta[CaloSize]</tt>: array of floats that give towers' @f$\eta@f$
 *   - <tt>CaloPhi[CaloSize]</tt>: array of floats that give towers' @f$\phi@f$
 *   - <tt>CaloEBHits[CaloSize]</tt>: array of integers that give towers' number
 *     of hits in the EB
 *   - <tt>CaloEEHits[CaloSize]</tt>: array of integers that give towers' number
 *     of hits in the EE
 *   - <tt>CaloHBHits[CaloSize]</tt>: array of integers that give towers' number
 *     of hits in the HB
 *   - <tt>CaloHEHits[CaloSize]</tt>: array of integers that give towers' number
 *     of hits in the HE
 *   - <tt>CaloHFHits[CaloSize]</tt>: array of integers that give towers' number
 *     of hits in the HF
 *   - <tt>CaloEmEnergy[CaloSize]</tt>: array of float that give towers'
 *     electromagnetic energy
 *   - <tt>CaloHadEnergy[CaloSize]</tt>: array of float that give towers'
 *     hadronic energy
 *   - <tt>CaloEnergy[CaloSize]</tt>: array of float that give towers' total
 *     energy
 *
 * An exception is thrown if @c tree is @c null (@c std::invalid_argument) or a
 * branch is missing (@c std::runtime_error). The @c towerset takes ownership of
 * the tree, and you shouldn't access it directly.
 */
towerset::towerset(TTree *tree) :
  _tree(tree)
{
  if (tree == nullptr) {
    throw std::invalid_argument("towerset::towerset: tree is null");
  }
  init_branches();
}

namespace {
  // Checks that the given branch exists in tree and sets its address to addr.
  void check_branch_and_set_address(TTree *tree, const char *name, void *addr)
  {
    TBranch *branch = tree->GetBranch(name);
    if (branch == nullptr) {
      std::string msg = "towerset::towerset: No branch named \"";
      msg += name;
      msg += "\" was found";
      throw std::runtime_error(msg);
    }
    branch->SetAddress(addr);
  }
}

void towerset::init_branches()
{
  // calo:: is needed because of a bug in CINT
  calo::check_branch_and_set_address(_tree, "CaloSize", &_size);
  _size = 0;
  calo::check_branch_and_set_address(_tree, "CaloEta", _eta);
  calo::check_branch_and_set_address(_tree, "CaloPhi", _phi);
  calo::check_branch_and_set_address(_tree, "CaloEBHits", _ebcount);
  calo::check_branch_and_set_address(_tree, "CaloEEHits", _eecount);
  calo::check_branch_and_set_address(_tree, "CaloHBHits", _hbcount);
  calo::check_branch_and_set_address(_tree, "CaloHEHits", _hecount);
  calo::check_branch_and_set_address(_tree, "CaloHFHits", _hfcount);
  calo::check_branch_and_set_address(_tree, "CaloEmEnergy", _emenergy);
  calo::check_branch_and_set_address(_tree, "CaloHadEnergy", _hadenergy);
  calo::check_branch_and_set_address(_tree, "CaloEnergy", _totalenergy);
}

/// Gets the given entry from the underlying @c TTree.
/**
 * @warning
 * This operation invalidates all iterators, so you must not use it while
 * iterators are alive. Use of invalid iterators is undefined behaviour (it
 * could, for example, trigger a beam dump).
 */
void towerset::getentry(unsigned long entry)
{
  _tree->GetEntry(entry);
}

/// Gets the number of entries in the underlying @c TTree.
unsigned long towerset::entries() const
{
  return _tree->GetEntries();
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
 *          1         |          0.36
 *          2         |          0.29
 *          3         |          0.26
 *          4         |          0.24
 *         >4         |          0.22
 */

/// Constructs a filter with the given thresholds
/**
 * The vector should contain the thresholds for @f$E_\mathrm{em}/N@f$. The first
 * element will be used when @f$N = 1@f$; the second when @f$N = 2@f$, and so
 * on. When no threshold is found in the vector (ie it is too short), the last
 * element is used.
 */
goodeb_filter::goodeb_filter(const std::vector<float> &thresholds) :
  _thresholds(thresholds)
{}

bool goodeb_filter::operator() (const tower_ref &tower) const
{
  if (tower.iseb()) {
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