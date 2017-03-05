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
#include "logic.h"
#include "eb.h"

// ROOT's pseudo-C++ parser
#ifdef __CINT__
# include "calofilter.cpp"
# include "eb.cpp"
#endif

int main()
{
  std::cout << "Running..." << std::endl;

  TFile *in = new TFile("../../../data/pPb_MinBias_2013_v5.root", "READ");
  in->cd();

  // There is a memory leak here, but it's ok as long as it stays outside of the
  // main loop.
  calo::and_filter f = calo::and_filter(&calo::goodeb,
                                        new calo::not_filter(&calo::coldeb));

  calo::towerset set;
  for (long entry = 0; entry < 100; ++entry) {
    set.getentry(entry);

    calo::towerset::iterator end = set.end();
    for (calo::towerset::iterator it = set.begin(&f); it != end; ++it) {
      std::cout << *it << " -> " << it->ebcount() << std::endl;
    }

// ROOT's pseudo-C++ parser doesn't support std::distance
#ifndef __CINT__
    std::cout << "There were "
              << std::distance(set.begin(&f), set.end())
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
