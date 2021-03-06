/**
 * @file
 * @brief  Source for the whole framework
 * @author Louis Moureaux
 * @date   2017
 */

#include "calofilter.h"

#include <cmath>
#include <stdexcept>
#include <string>

#include <TBranch.h>
#include <TDirectory.h>
#include <TTree.h>

// At least ROOT doesn't define it in cmath
#ifndef M_PI
/// A macro for @f$ \pi @f$ (not all @c cmath headers define it)
# define M_PI 3.141592653589793238462643383279502884
#endif

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
 * All examples below assume that you imported the full namespace. If you don't
 * want to do it, add @c calo:: before all class names.
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
 * @subsection Filtering Filtering
 *
 * Filtering towers is done by passing a @ref calo::filter object to
 * @ref calo::towerset::begin "begin". The resulting iterator will then return
 * towers that are deemed "good" by the filter.
 *
 * Some filters are defined by default; here, we will use
 * @ref calo::goodeb_filter "goodeb" to remove the noise from the barrel of the
 * electromagnetic calorimeter. First, we need to @c #include the EB filters:
 *
 * ~~~~{.cpp}
 * #include "eb.cpp" // .h if linking to the static library
 * ~~~~
 *
 * We may then use @c goodeb in our loop statement:
 *
 * ~~~~{.cpp}
 * // For each "good" tower in the EB
 * const towerset::iterator end = tset.end();
 * for (towerset::iterator it = tset.begin(&goodeb); it != end; ++it) {
 *   // ...
 * }
 * ~~~~
 *
 * @subsection Counting Counting
 *
 * You may just be interested in whether there are towers, not in their
 * individual properties. In this case, the shortest code you can write is the
 * following:
 *
 * ~~~~{.cpp}
 * bool is_there_a_tower = (tset.begin() != tset.end());
 * ~~~~
 *
 * Notice that we used the same @c begin function as for loops. It is therefore
 * straighforward to require, for example, that there is no good EB tower:
 *
 * ~~~~{.cpp}
 * bool is_there_a_tower = (tset.begin(&goodeb) != tset.end());
 * ~~~~
 *
 * Another common operation is to count how many towers there are. @c C++
 * defines a function that does just that, @c std::distance. It is used as
 * follows:
 *
 * ~~~~{.cpp}
 * int count = std::distance(tset.begin(), tset.end());
 * ~~~~
 *
 * By now, you should have understood that, if you pass a filter to
 * <tt>begin()</tt>, you will get the number of towers that pass the filter.
 *
 * Unfortunately, not all ROOT versions support @c std::distance. If yours
 * doesn't, you can use a plain old loop. (Or better: define your own
 * @c distance function.)
 *
 * @section Advanced Advanced topics
 *
 * @subsection OwnFilters Defining your own filters
 *
 * If you want to define a new filter, the easiest way is to start from an
 * existing one and change only what's needed. In this section, you will learn
 * how to build a new one from scratch.
 *
 * For the sake of the example, we will define a filter that selects towers
 * with @f$|\eta| < \eta_\mathrm{max}@f$. We start by creating a new class
 * derived from @ref calo::filter (put it between @c #includes and your first
 * function):
 *
 * ~~~~{.cpp}
 * class example_filter : public filter
 * {
 * public:
 * ~~~~
 *
 * Since this is an example, we won't use proper encapsulation. We define a
 * variable for @f$\eta_\mathrm{max}@f$ and a simple constructor:
 *
 * ~~~~{.cpp}
 *   float etamax;
 *   explicit example_filter(float etamax) : etamax(etamax) {}
 * ~~~~
 *
 * The actual filtering happens in <tt>operator()</tt>:
 *
 * ~~~~{.cpp}
 *   bool operator() (const tower_ref &tower) const
 *   {
 *     return std::abs(tower.eta()) < etamax;
 *   }
 * ~~~~
 *
 * Note that the signature of that function should be exactly as above (you can
 * only change the name of the argument). In particular, the @c const keyword
 * is important.
 *
 * That's all we need, so we can end the class here:
 *
 * ~~~~{.cpp}
 * }; // Don't forget the semicolon
 * ~~~~
 *
 * Before we can use our filter, we need to create a variable of that type:
 *
 * ~~~~{.cpp}
 * example_filter exfilter(2.0); // Sets etamax to 2.0
 * ~~~~
 *
 * Using it is then simple done as usual:
 *
 * ~~~~{.cpp}
 * const towerset::iterator end = tset.end();
 * for (towerset::iterator it = tset.begin(&exfilter); it != end; ++it) {
 *   // ...
 * }
 * ~~~~
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

} // namespace calo