#ifndef CALOFILTER_H
#define CALOFILTER_H

#include <cassert>
#include <iostream>
#include <iterator>

#if __cplusplus < 201103L
# define nullptr 0
#endif

// ROOT's pseudo-C++ parser
#ifdef __CINT__
# define nullptr 0
#endif

class TTree;
class TDirectory;

/// The namespace for calorimeters
namespace calo
{

class tower_ref;

/// Towers are @c reco objects that hold information about hits.
/**
 * In the barrel, each tower corresponds to a group of 5 by 5 ECAL crystals and
 * one HCAL cell.
 */
class tower
{
  float _eta;
  float _phi;

  int _ebcount;
  int _eecount;
  int _hbcount;
  int _hecount;
  int _hfcount;

  float _emenergy;
  float _hadenergy;
  float _totalenergy;

public:
  /// Constructs an empty tower
  explicit tower() {}

  /// Copy constructor
  inline tower(const tower &other);

  /// Creates a @c tower from a @ref tower_ref
  inline tower(const tower_ref &ref);

  /// Compares two towers for equality
  inline bool operator== (const tower &other) const;

  /// Compares two towers for inequality
  bool operator!= (const tower &other) const { return !(*this == other); }

  /// Returns the mean @f$\eta@f$ for this tower
  float eta() const { return _eta; }

  /// Returns the mean @f$\phi@f$ for this tower
  float phi() const { return _phi; }

  /// Returns the number of EB crystals that were taken into account when
  /// building the tower
  int ebcount() const { return _ebcount; }

  /// Returns the number of EE crystals that were taken into account when
  /// building the tower
  int eecount() const { return _eecount; }

  /// Returns the number of HB cells that were taken into account when
  /// building the tower
  int hbcount() const { return _hbcount; }

  /// Returns the number of HE cells that were taken into account when
  /// building the tower
  int hecount() const { return _hecount; }

  /// Returns the number of HF cells that were taken into account when
  /// building the tower
  int hfcount() const { return _hfcount; }

  /// Returns @c true if EB was used to build the tower
  bool iseb() const { return ebcount() > 0; }

  /// Returns @c true if EE was used to build the tower
  bool isee() const { return eecount() > 0; }

  /// Returns @c true if HB was used to build the tower
  bool ishb() const { return hbcount() > 0; }

  /// Returns @c true if HE was used to build the tower
  bool ishe() const { return hecount() > 0; }

  /// Returns @c true if HF was used to build the tower
  bool ishf() const { return hfcount() > 0; }

  /// Returns the energy deposited in the electromagnetic calorimeters
  float emenergy() const { return _emenergy; }

  /// Returns the energy deposited in the hadronic calorimeters
  float hadenergy() const { return _hadenergy; }

  /// Returns the total energy deposited in all calorimeters
  float totalenergy() const { return _totalenergy; }
};

std::ostream &operator<< (std::ostream &out, const tower &t);

class towerset;

/// Zero-copy version of @ref tower
/**
 * You should only use this class through @ref towerset::iterator. It is an
 * optimization for the common case where storing the tower isn't needed.
 * Using @c tower_ref variables isn't supported.
 */
class tower_ref
{
  const towerset *_set;
  int _i;

public:
  explicit tower_ref() {}
  inline explicit tower_ref(const towerset *c, int index);

  inline bool operator== (const tower_ref &other) const;
  inline bool operator== (const tower &other) const;
  bool operator!= (const tower_ref &other) const { return !(*this == other); }
  bool operator!= (const tower &other) const { return !(*this == other); }

  inline float eta() const;
  inline float phi() const;

  inline int ebcount() const;
  inline int eecount() const;
  inline int hbcount() const;
  inline int hecount() const;
  inline int hfcount() const;

  bool iseb() const { return ebcount() > 0; }
  bool isee() const { return eecount() > 0; }
  bool ishb() const { return hbcount() > 0; }
  bool ishe() const { return hecount() > 0; }
  bool ishf() const { return hfcount() > 0; }

  inline float emenergy() const;
  inline float hadenergy() const;
  inline float totalenergy() const;
};

/// Base class for tower filters.
/**
 * Custom filters should overload the @c const version of <tt>operator()</tt>,
 * which should return @c true if the tower passes the filter (ie, it should be
 * taken into account).
 *
 * An example of a simple filter is given below:
 *
 * ~~~~{.cpp}
 * class etafilter : public filter
 * {
 * public:
 *   bool operator() (const tower_ref &tower) const
 *   {
 *     // Only keep towers with positive eta
 *     return tower.eta() > 0;
 *   }
 * };
 * ~~~~
 *
 * One would use this filter as follows:
 *
 * ~~~~{.cpp}
 * etafilter filter;
 * calo::towerset::iterator end = set.end();
 * for (calo::towerset::iterator it = set.begin(&filter); it != end; ++it) {
 *   // Do your stuff here
 * }
 * ~~~~
 */
class filter
{
public:
  /// Returns @c true if the tower passes the filter
  virtual bool operator() (const tower_ref &) const = 0;
};

/// A collection of all towers in an event.
class towerset
{
  friend class tower_ref;

  /// A filter that lets every tower pass
  class nofilter : public filter
  {
  public:
    /// Returns @c true
    bool operator() (const tower_ref &) const { return true; }
  };

public:
  /// Iterator over towers in a @ref towerset
  /**
   * This class implements a standard-compliant bidirectional iterator that
   * iterates over all towers in a @ref towerset.
   *
   * @see The @ref towerset class and the @c iterator_traits specialization
   *      @ref std::iterator_traits<calo::towerset::iterator>.
   */
  class iterator
  {
    friend class towerset;

    const towerset *_set;
    const filter *_filter;
    int _i;
    tower_ref _t;

    inline void set(const towerset *set, int index, const filter *filter);

    inline void step_forward();
    inline void step_backward();

  public:
    /// Access the tower referenced by the iterator
    const tower_ref &operator* () const { return _t; }

    /// Access members of the tower referenced by the iterator
    const tower_ref *operator-> () const { return &_t; }

    inline iterator &operator++ ();
    inline iterator operator++ (int);
    inline iterator &operator-- ();
    inline iterator operator-- (int);

    /// Compares two iterators for equality
    bool operator== (const iterator &other) { return _t == other._t; }

    /// Compares two iterators for inequality
    bool operator!= (const iterator &other) { return !(*this == other); }
  };

private:
  static const unsigned int big = 1000;

  TTree *_tree;

  nofilter _nofilter; // ROOT doesn't work well with static variables

  int _size;

  float _eta[big];
  float _phi[big];

  int _ebcount[big];
  int _eecount[big];
  int _hbcount[big];
  int _hecount[big];
  int _hfcount[big];

  float _emenergy[big];
  float _hadenergy[big];
  float _totalenergy[big];

  void init_branches();

public:
  explicit towerset();
  explicit towerset(TTree *tree);
  explicit towerset(TDirectory *dir);
  virtual ~towerset() {}

  void getentry(unsigned long entry);
  unsigned long entries() const;

  inline iterator begin(const filter *filter = nullptr) const;
  inline iterator end() const;
};

} // namespace calo

////////////////////////////////////////////////////////////////////////////////

namespace std
{

/**
 * @brief
 * Specialization needed for calo::towerset::iterator to be a standard-compliant
 * bidirectional iterator.
 */
template<>
struct iterator_traits<calo::towerset::iterator>
{
  typedef int difference_type;
  typedef calo::tower_ref value_type;
  typedef calo::tower_ref *pointer;
  typedef calo::tower_ref &reference;
  typedef std::bidirectional_iterator_tag iterator_setategory;
};

} // namespace std

////////////////////////////////////////////////////////////////////////////////

namespace calo
{

tower::tower(const tower &other) :
  _eta(other.eta()),
  _phi(other.phi()),
  _ebcount(other.ebcount()),
  _eecount(other.eecount()),
  _hbcount(other.hbcount()),
  _hecount(other.hecount()),
  _hfcount(other.hfcount()),
  _emenergy(other.emenergy()),
  _hadenergy(other.hadenergy()),
  _totalenergy(other.totalenergy())
{}

tower::tower(const tower_ref &ref) :
  _eta(ref.eta()),
  _phi(ref.phi()),
  _ebcount(ref.ebcount()),
  _eecount(ref.eecount()),
  _hbcount(ref.hbcount()),
  _hecount(ref.hecount()),
  _hfcount(ref.hfcount()),
  _emenergy(ref.emenergy()),
  _hadenergy(ref.hadenergy()),
  _totalenergy(ref.totalenergy())
{}

bool tower::operator== (const tower &other) const
{
  return eta() == other.eta()
      && phi() == other.phi()
      && ebcount() == other.ebcount()
      && eecount() == other.eecount()
      && hbcount() == other.hbcount()
      && hecount() == other.hecount()
      && hfcount() == other.hfcount()
      && emenergy() == other.emenergy()
      && hadenergy() == other.hadenergy()
      && totalenergy() == other.totalenergy();
}

bool tower_ref::operator== (const tower_ref &other) const
{
  return _set == other._set && _i == other._i;
}

bool tower_ref::operator== (const tower &other) const
{
  return eta() == other.eta()
      && phi() == other.phi()
      && ebcount() == other.ebcount()
      && eecount() == other.eecount()
      && hbcount() == other.hbcount()
      && hecount() == other.hecount()
      && hfcount() == other.hfcount()
      && emenergy() == other.emenergy()
      && hadenergy() == other.hadenergy()
      && totalenergy() == other.totalenergy();
}

tower_ref::tower_ref(const towerset *set, int index) :
  _set(set), _i(index)
{
  assert(set != nullptr);
  assert(index >= 0);
  assert(index <= set->_size);
}

float tower_ref::eta() const
{
  assert(_i < _set->_size);
  return _set->_eta[_i];
}

float tower_ref::phi() const
{
  assert(_i < _set->_size);
  return _set->_phi[_i];
}

int tower_ref::ebcount() const
{
  assert(_i < _set->_size);
  return _set->_ebcount[_i];
}

int tower_ref::eecount() const
{
  assert(_i < _set->_size);
  return _set->_eecount[_i];
}

int tower_ref::hbcount() const
{
  assert(_i < _set->_size);
  return _set->_hbcount[_i];
}

int tower_ref::hecount() const
{
  assert(_i < _set->_size);
  return _set->_hecount[_i];
}

int tower_ref::hfcount() const
{
  assert(_i < _set->_size);
   return _set->_hfcount[_i];
}

float tower_ref::emenergy() const
{
  assert(_i < _set->_size);
  return _set->_emenergy[_i];
}

float tower_ref::hadenergy() const
{
  assert(_i < _set->_size);
  return _set->_hadenergy[_i];
}

float tower_ref::totalenergy() const
{
  assert(_i < _set->_size);
  return _set->_totalenergy[_i];
}

/// Sets the value pointed to by the iterator
void towerset::iterator::set(const towerset *set,
                             int index,
                             const filter *filter)
{
  _set = set;
  _i = index;
  _t = tower_ref(set, index);
  _filter = filter;
  step_forward();
}

/// Finds the next good index (current included)
void towerset::iterator::step_forward()
{
  const filter &f = *_filter;
  while (_i < _set->_size && !f(_t)) {
    _t = tower_ref(_set, ++_i);
  }
}

/// Finds the previous good index (current included)
void towerset::iterator::step_backward()
{
  const filter &f = *_filter;
  while (_i >= 0 && !f(_t)) {
    _t = tower_ref(_set, --_i);
  }
}

/// Prefix increment operator
towerset::iterator &towerset::iterator::operator++ ()
{
  _t = tower_ref(_set, ++_i);
  step_forward();
  return *this;
}

/// Postfix increment operator
towerset::iterator towerset::iterator::operator++ (int)
{
  iterator temp = *this;
  ++*this;
  return temp;
}

/// Prefix decrement operator
towerset::iterator &towerset::iterator::operator-- ()
{
  _t = tower_ref(_set, --_i);
  return *this;
}

/// Postfix decrement operator
towerset::iterator towerset::iterator::operator-- (int)
{
  iterator temp = *this;
  --*this;
  return temp;
}

/// Returns an iterator referencing the first tower
/**
 * If @c filter is given, the resulting object will iterate only over towers for
 * which <tt>filter()</tt> returns @c true, else it will iterate over all
 * towers.
 */
towerset::iterator towerset::begin(const filter *filter) const
{
  if (filter == nullptr) {
    filter = &_nofilter;
  }
  iterator it;
  it.set(this, 0, filter);
  return it;
}

/// Returns a past-the-end iterator
towerset::iterator towerset::end() const
{
  iterator it;
  it.set(this, _size, &_nofilter);
  return it;
}

} // namespace calo

#endif // CALOFILTER_H
