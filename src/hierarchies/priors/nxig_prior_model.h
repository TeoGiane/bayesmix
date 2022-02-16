#ifndef BAYESMIX_HIERARCHIES_PRIORS_NXIG_PRIOR_MODEL_H_
#define BAYESMIX_HIERARCHIES_PRIORS_NXIG_PRIOR_MODEL_H_

// #include <google/protobuf/stubs/casts.h>

#include <Eigen/Dense>
#include <memory>
#include <stan/math/prim.hpp>
#include <vector>

// #include "algorithm_state.pb.h"
#include "base_prior_model.h"
#include "hierarchy_prior.pb.h"
#include "hyperparams.h"
#include "src/utils/rng.h"

class NxIGPriorModel : public BasePriorModel<NxIGPriorModel, Hyperparams::NxIG,
                                            bayesmix::NNxIGPrior> {
 public:
  NxIGPriorModel() = default;
  ~NxIGPriorModel() = default;

  double lpdf(const google::protobuf::Message &state_) override;

  std::shared_ptr<google::protobuf::Message> sample(
      bool use_post_hypers) override;

  void update_hypers(const std::vector<bayesmix::AlgorithmState::ClusterState>
                         &states) override;

  void set_hypers_from_proto(
      const google::protobuf::Message &hypers_) override;

 protected:
  std::shared_ptr<bayesmix::AlgorithmState::HierarchyHypers> get_hypers_proto()
      const override;

  void initialize_hypers() override;
};

#endif  // BAYESMIX_HIERARCHIES_PRIORS_NXIG_PRIOR_MODEL_H_
