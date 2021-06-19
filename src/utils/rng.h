#ifndef BAYESMIX_UTILS_RNG_H_
#define BAYESMIX_UTILS_RNG_H_

#include <random>

//! Simple Random Number Generation class wrapper.

//! This class wraps the C++ standard RNG object and allows the use of any RNG
//! seed. It is implemented as a singleton, so that every object used in the
//! library has access to the same exact RNG engine. (TODO explain why)

namespace bayesmix {
class Rng {
 public:
  //! Returns (and creates if nonexistent) the singleton of this class
  static Rng &Instance() {
    static Rng s;
    return s;
  }

  //! Returns a reference to the underlying RNG object
  std::mt19937_64 &get() { return mt; }

  //! Sets the RNG seed
  void seed(int seed_val) { mt.seed(seed_val); }

 private:
  Rng(int seed_val = 20201103) { mt.seed(seed_val); }
  ~Rng() {}

  Rng(Rng const &) = delete;
  Rng &operator=(Rng const &) = delete;

  //! C++ standard library RNG object
  std::mt19937_64 mt;
};
}  // namespace bayesmix

#endif  // BAYESMIX_UTILS_RNG_H_
