#include <iostream>
#include <TFile.h>

#include "calofilter.h"

// ROOT's pseudo-C++ parser
#ifdef __CINT__
# include "calofilter.cpp"
#endif

int main()
{
  std::cout << "Running..." << std::endl;

  TFile *in = new TFile("../data/pPb_MinBias_2013_v5.root", "READ");
  in->cd();

  calo::towerset set;
  set.getentry(0);

  calo::towerset::iterator end = set.end();
  for (calo::towerset::iterator it = set.begin(); it != end; ++it) {
    std::cout << *it << " -> " << it->ebcount() << std::endl;
  }

  std::cout << "Finished!" << std::endl;
  return 0;
}

// ROOT's pseudo-C++ parser
#ifdef __CINT__
void test() { main(); }
#endif
