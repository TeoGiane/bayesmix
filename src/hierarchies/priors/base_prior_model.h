#ifndef BAYESMIX_HIERARCHIES_PRIORS_BASE_PRIOR_MODEL_H_
#define BAYESMIX_HIERARCHIES_PRIORS_BASE_PRIOR_MODEL_H_

#include <google/protobuf/message.h>

#include <memory>
#include <random>
#include <set>
#include <stan/math/prim.hpp>
#include <stan/math/rev.hpp>

#include "abstract_prior_model.h"
#include "algorithm_state.pb.h"
#include "hierarchy_id.pb.h"
#include "src/utils/rng.h"

template <class Derived, typename HyperParams, typename Prior>
class BasePriorModel : public AbstractPriorModel {
 public:
  BasePriorModel() = default;

  ~BasePriorModel() = default;

  double lpdf_from_unconstrained(
      Eigen::VectorXd unconstrained_params) const override {
    return static_cast<const Derived &>(*this)
        .template lpdf_from_unconstrained<double>(unconstrained_params);
  }

  stan::math::var lpdf_from_unconstrained(
      Eigen::Matrix<stan::math::var, Eigen::Dynamic, 1> unconstrained_params)
      const override {
    return static_cast<const Derived &>(*this)
        .template lpdf_from_unconstrained<stan::math::var>(
            unconstrained_params);
  }

  virtual std::shared_ptr<AbstractPriorModel> clone() const override;

  virtual google::protobuf::Message *get_mutable_prior() override;

  HyperParams get_hypers() const { return hypers; }

  HyperParams get_posterior_hypers() const { return post_hypers; }

  void set_posterior_hypers(const HyperParams &_post_hypers) {
    post_hypers = _post_hypers;
  };

  void write_hypers_to_proto(google::protobuf::Message *out) const override;

  void initialize();

 protected:
  void check_prior_is_set() const;

  void create_empty_prior() { prior.reset(new Prior); }

  bayesmix::AlgorithmState::HierarchyHypers *downcast_hypers(
      google::protobuf::Message *state_) const {
    return google::protobuf::internal::down_cast<
        bayesmix::AlgorithmState::HierarchyHypers *>(state_);
  }

  const bayesmix::AlgorithmState::HierarchyHypers &downcast_hypers(
      const google::protobuf::Message &state_) const {
    return google::protobuf::internal::down_cast<
        const bayesmix::AlgorithmState::HierarchyHypers &>(state_);
  }

  const bayesmix::AlgorithmState::ClusterState &downcast_state(
      const google::protobuf::Message &state_) const {
    return google::protobuf::internal::down_cast<
        const bayesmix::AlgorithmState::ClusterState &>(state_);
  }

  HyperParams hypers;
  HyperParams post_hypers;
  std::shared_ptr<Prior> prior;
};

// Methods Definitions

template <class Derived, typename HyperParams, typename Prior>
std::shared_ptr<AbstractPriorModel>
BasePriorModel<Derived, HyperParams, Prior>::clone() const {
  auto out = std::make_shared<Derived>(static_cast<Derived const &>(*this));
  return out;
}

template <class Derived, typename HyperParams, typename Prior>
google::protobuf::Message *
BasePriorModel<Derived, HyperParams, Prior>::get_mutable_prior() {
  if (prior == nullptr) {
    create_empty_prior();
  }
  return prior.get();
}

template <class Derived, typename HyperParams, typename Prior>
void BasePriorModel<Derived, HyperParams, Prior>::write_hypers_to_proto(
    google::protobuf::Message *out) const {
  std::shared_ptr<bayesmix::AlgorithmState::HierarchyHypers> hypers_ =
      get_hypers_proto();
  auto *out_cast = downcast_hypers(out);
  out_cast->CopyFrom(*hypers_.get());
}

template <class Derived, typename HyperParams, typename Prior>
void BasePriorModel<Derived, HyperParams, Prior>::initialize() {
  check_prior_is_set();
  initialize_hypers();
}

template <class Derived, typename HyperParams, typename Prior>
void BasePriorModel<Derived, HyperParams, Prior>::check_prior_is_set() const {
  if (prior == nullptr) {
    throw std::invalid_argument("Hierarchy prior was not provided");
  }
}

#endif  // BAYESMIX_HIERARCHIES_PRIORS_BASE_PRIOR_MODEL_H_
