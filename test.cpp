/**
 * @file
 * @brief  Test file (but also an example for the API
 * @author Louis Moureaux
 * @date   2017
 */

#include <iostream>
#include <iterator>
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
  for (long entry = 0; entry < 100; ++entry) {
    set.getentry(entry);

    calo::towerset::iterator end = set.end();
    for (calo::towerset::iterator it = set.begin(&calo::goodeb);
         it != end; ++it) {
      std::cout << *it << " -> " << it->ebcount() << std::endl;
    }

// ROOT's pseudo-C++ parser doesn't support std::distance
#ifndef __CINT__
    std::cout << "There were "
              << std::distance(set.begin(&calo::goodeb), set.end())
              << " towers passing the filter." << std::endl;
#endif
  }

  std::cout << "Finished!" << std::endl;
  return 0;
}

// ROOT's pseudo-C++ parser
#ifdef __CINT__
void test() { main(); }
#endif
