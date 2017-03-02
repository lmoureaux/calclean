/**
 * @file
 * @brief  Program to determine HB thresholds
 * @author Louis Moureaux
 * @date   2017
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <TFile.h>

#include "calofilter.h"
#include "hb.h"

/// The number of bins in @f$\eta@f$.
const int ETA_DIVS = 34;

void printusage(const char *progname);

/// Holds the configuration.
class config
{
  double _pvalue;
  int _numhot[ETA_DIVS];
  std::string errmsg(int linenb, const std::string &descr) const;
  double strtod(int linenb, const std::string &word) const;
  long strtol(int linenb, const std::string &word) const;
public:
  explicit config(std::istream &in);
  /// Returns the target pvalue.
  double pvalue() const { return _pvalue; }
  /// Returns the target number of hot cells for the given eta bin.
  double numhot(int ieta) const { return _numhot[ieta + ETA_DIVS / 2]; }
};

/// Entry point for the program.
int main(int argc, char *argv[])
{
  // Do we have enough arguments?
  if (argc < 3) {
    std::cerr << "Error: Not enough parameters" << std::endl;
    printusage(argv[0]);
    std::exit(1);
  }

  // Try to open the config file
  std::ifstream input;
  try {
    input.exceptions(std::ifstream::failbit);
    input.open(argv[1]);
    input.exceptions(std::ifstream::goodbit);
  } catch (std::exception &e) {
    std::cerr << "Error: cannot open '" << argv[1] << "': "
              << e.what() << std::endl;
    std::exit(1);
  }

  // Try to read its contents
  config *c = nullptr;
  try {
    c = new config(input);
    input.close();
  } catch (std::exception &e) {
    std::cerr << "Error: cannot read configuration from '" << argv[1] << "': "
              << e.what() << std::endl;
    std::exit(1);
  }

  for (int ieta = -ETA_DIVS / 2; ieta < ETA_DIVS / 2; ++ieta) {
    std::cout << ieta << " " << c->numhot(ieta) << std::endl;
  }

  TFile *in = new TFile(argv[2], "READ");
  if (in->GetErrno() != 0) {
    // TFile prints its own error message
    std::exit(1);
  }
  in->cd();

  return 0;
}

/// Prints a short help message.
void printusage(const char *progname)
{
  std::cerr << "Usage: " << progname << " <input> <data.root>" << std::endl;
  std::cerr << "Where:" << std::endl;
  std::cerr << "  <input> is a file that contains the number of cells to remove"
             "\n          in every eta bin" << std::endl;
  std::cerr << "  <data.root> contains a valid calofilter tree" << std::endl;
}

/// Reads values from the given stream.
config::config(std::istream &in) :
  _pvalue(0.01)
{
  for (int linenb = 1; in; ++linenb) {
    std::string line;
    std::getline(in, line);
    std::stringstream lstream(line);

    // Divide into words
    std::vector<std::string> words;
    while (lstream) {
      std::string word;
      lstream >> word;
      if (word.empty() || word[0] == '#') {
        break;
      }
      words.push_back(word);
    }

    // Skip empty lines and comments
    if (words.empty()) {
      continue;
    } else if (words.size() > 0 && words.size() != 2) {
      throw std::runtime_error(errmsg(linenb, "incomplete statement"));
    }

    if (words.front() == "pvalue") {
      // pvalue <number>
      _pvalue = strtod(linenb, words.back());
      if (_pvalue <= 0 || _pvalue > 1) {
        throw std::runtime_error("pvalue must be in (0, 1]");
      }
    } else {
      // <eta> <hotcount>
      long ieta;
      try {
        ieta = strtol(linenb, words.front());
      } catch (...) {
        std::string descr = "unknown parameter '";
        descr += words.front();
        descr += "'";
        throw std::runtime_error(errmsg(linenb, descr));
      }
      ieta += ETA_DIVS / 2;
      // Prevent segfaults
      if (ieta < 0 || ieta >= ETA_DIVS) {
        throw std::runtime_error(errmsg(linenb, "ieta out of bounds"));
      }
      _numhot[ieta] = strtol(linenb, words.back());
      if (_numhot[ieta] < 0) {
        throw std::runtime_error("value must be positive");
      }
    }
  }
}

/// Formats an error message for the given line number
std::string config::errmsg(int linenb, const std::string &descr) const
{
  std::stringstream msg;
  msg << "line " << linenb << ": " << descr;
  return msg.str();
}

/// Converts a string to a @c double, printing the line number if an error
/// occurs
double config::strtod(int linenb, const std::string &word) const
{
  const char *start = &word[0];
  char *end;
  double ret = std::strtod(start, &end);
  if (start == end || (int) word.size() != end - start) {
    std::stringstream msg;
    msg << "cannot interpret '" << word << "' as a number";
    throw std::runtime_error(errmsg(linenb, msg.str()));
  }
  return ret;
}

/// Converts a string to a @c long, printing the line number if an error occurs
long config::strtol(int linenb, const std::string &word) const
{
  const char *start = &word[0];
  char *end;
  long ret = std::strtol(start, &end, 10);
  if (start == end || (int) word.size() != end - start) {
    std::stringstream msg;
    msg << "cannot interpret '" << word << "' as a number";
    throw std::runtime_error(errmsg(linenb, msg.str()));
  }
  return ret;
}
