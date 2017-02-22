#include "calofilter.h"

#include <stdexcept>
#include <string>

#include <TBranch.h>
#include <TDirectory.h>
#include <TTree.h>

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