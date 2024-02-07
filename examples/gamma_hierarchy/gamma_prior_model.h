#ifndef BAYESMIX_HIERARCHIES_GAMMA_PRIOR_MODEL_H_
#define BAYESMIX_HIERARCHIES_GAMMA_PRIOR_MODEL_H_

#include <memory>
#include <stan/math/prim.hpp>
#include <stan/math/rev.hpp>
#include <vector>

#include "algorithm_state.pb.h"
#include "examples/gamma_gamma_prior.pb.h"
#include "gamma_likelihood.h"
#include "gamma_utils.h"
#include "hierarchy_prior.pb.h"
#include "src/hierarchies/priors/base_prior_model.h"
#include "src/utils/rng.h"

namespace Hyperparams {
struct GammaGamma {
  double shape, rate_alpha, rate_beta;
};
}  // namespace Hyperparams

class GammaPriorModel : public BasePriorModel<GammaPriorModel, State::Gamma,
                                              Hyperparams::GammaGamma,
                                              bayesmix::GammaGammaPrior> {
 public:
  using AbstractPriorModel::ProtoHypers;
  using AbstractPriorModel::ProtoHypersPtr;
  GammaPriorModel() = default;
  ~GammaPriorModel() = default;
  double lpdf(const google::protobuf::Message &state_) override;
  State::Gamma sample(ProtoHypersPtr hier_hypers = nullptr) override;
  void update_hypers(const std::vector<bayesmix::AlgorithmState::ClusterState>
                         &states) override;
  void set_hypers_from_proto(
      const google::protobuf::Message &hypers_) override;
  ProtoHypersPtr get_hypers_proto() const override;

 protected:
  void initialize_hypers() override;
};

/* DEFINITIONS */
double GammaPriorModel::lpdf(const google::protobuf::Message &state_) {
  // Downcast state
  auto statecast = downcast_state(state_).custom_state();
  // Unpack state
  auto unpacked_statecast =
      bayesmix::unpack_protobuf_any<bayesmix::GammaState>(statecast);
  return stan::math::gamma_lpdf(unpacked_statecast.rate(), hypers->rate_alpha,
                                hypers->rate_beta);
}

State::Gamma GammaPriorModel::sample(ProtoHypersPtr hier_hypers) {
  // RNG
  auto &rng = bayesmix::Rng::Instance().get();
  // Get proper params
  auto params = (hier_hypers) ? hier_hypers->custom_state()
                              : get_hypers_proto()->custom_state();
  // Unpack params
  auto unpacked_params =
      bayesmix::unpack_protobuf_any<bayesmix::GammaGammaDistribution>(params);
  // Compute output
  State::Gamma out;
  out.shape = unpacked_params.shape();
  out.rate = stan::math::gamma_rng(unpacked_params.rate_alpha(),
                                   unpacked_params.rate_beta(), rng);
  return out;
}

void GammaPriorModel::update_hypers(
    const std::vector<bayesmix::AlgorithmState::ClusterState> &states) {
  if (prior->has_fixed_values()) {
    return;
  } else {
    throw std::invalid_argument("Unrecognized hierarchy prior");
  }
}

void GammaPriorModel::set_hypers_from_proto(
    const google::protobuf::Message &hypers_) {
  // Downcast hypers
  auto &hyperscast = downcast_hypers(hypers_).custom_state();
  // Unpack hypers
  auto unpacked_hyperscast =
      bayesmix::unpack_protobuf_any<bayesmix::GammaGammaDistribution>(
          hyperscast);
  // Set hypers
  hypers->shape = unpacked_hyperscast.shape();
  hypers->rate_alpha = unpacked_hyperscast.rate_alpha();
  hypers->rate_beta = unpacked_hyperscast.rate_beta();
};

GammaPriorModel::ProtoHypersPtr GammaPriorModel::get_hypers_proto() const {
  ProtoHypersPtr out = std::make_shared<ProtoHypers>();
  bayesmix::GammaGammaDistribution unpacked_hypers;
  unpacked_hypers.set_shape(hypers->shape);
  unpacked_hypers.set_rate_alpha(hypers->rate_alpha);
  unpacked_hypers.set_rate_beta(hypers->rate_beta);
  out->mutable_custom_state()->PackFrom(unpacked_hypers);
  return out;
};

void GammaPriorModel::initialize_hypers() {
  if (prior->has_fixed_values()) {
    hypers->shape = prior->fixed_values().shape();
    hypers->rate_alpha = prior->fixed_values().rate_alpha();
    hypers->rate_beta = prior->fixed_values().rate_beta();
  } else {
    throw std::runtime_error("Unrecognized hierarchy prior.");
  }
  // Checks
  if (hypers->shape <= 0) {
    throw std::runtime_error("shape must be positive");
  }
  if (hypers->rate_alpha <= 0) {
    throw std::runtime_error("rate_alpha must be positive");
  }
  if (hypers->rate_beta <= 0) {
    throw std::runtime_error("rate_beta must be positive");
  }
}

#endif  // BAYESMIX_HIERARCHIES_GAMMA_PRIOR_MODEL_H_
