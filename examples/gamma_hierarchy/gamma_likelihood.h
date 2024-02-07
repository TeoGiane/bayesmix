#ifndef BAYESMIX_HIERARCHIES_GAMMA_LIKELIHOOD_H_
#define BAYESMIX_HIERARCHIES_GAMMA_LIKELIHOOD_H_

#include <google/protobuf/stubs/casts.h>

#include <memory>
#include <stan/math/rev.hpp>
#include <vector>

#include "algorithm_state.pb.h"
#include "examples/gamma_state.pb.h"
#include "gamma_utils.h"
#include "src/hierarchies/likelihoods/base_likelihood.h"
#include "src/hierarchies/likelihoods/states/base_state.h"

namespace State {
class Gamma : public BaseState {
 public:
  double shape, rate;
  using ProtoState = bayesmix::AlgorithmState::ClusterState;

  ProtoState get_as_proto() const override {
    // Fill ProtoState object
    ProtoState out;
    bayesmix::GammaState state;
    state.set_shape(shape);
    state.set_rate(rate);
    out.mutable_custom_state()->PackFrom(state);
    return out;
  }

  void set_from_proto(const ProtoState &state_, bool update_card) override {
    // Set cardinality
    if (update_card) {
      card = state_.cardinality();
    }
    // Unpack state
    auto unpacked_state = bayesmix::unpack_protobuf_any<bayesmix::GammaState>(
        state_.custom_state());
    // Set shape and rate
    shape = unpacked_state.shape();
    rate = unpacked_state.rate();
  }
};
}  // namespace State

class GammaLikelihood : public BaseLikelihood<GammaLikelihood, State::Gamma> {
 public:
  GammaLikelihood() = default;
  ~GammaLikelihood() = default;
  bool is_multivariate() const override { return false; };
  bool is_dependent() const override { return false; };
  void clear_summary_statistics() override;
  double get_data_sum() const { return data_sum; };

 protected:
  double compute_lpdf(const Eigen::RowVectorXd &datum) const override;
  void update_sum_stats(const Eigen::RowVectorXd &datum, bool add) override;

  //! Sum of data in the cluster
  double data_sum = 0;
};

/* DEFINITIONS */
void GammaLikelihood::clear_summary_statistics() { data_sum = 0; }

double GammaLikelihood::compute_lpdf(const Eigen::RowVectorXd &datum) const {
  return stan::math::gamma_lpdf(datum(0), state.shape, state.rate);
}

void GammaLikelihood::update_sum_stats(const Eigen::RowVectorXd &datum,
                                       bool add) {
  if (add) {
    data_sum += datum(0);
  } else {
    data_sum -= datum(0);
  }
}

#endif  // BAYESMIX_HIERARCHIES_GAMMA_LIKELIHOOD_H_
