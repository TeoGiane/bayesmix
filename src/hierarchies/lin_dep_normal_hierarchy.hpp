#ifndef BAYESMIX_HIERARCHIES_LIN_DEP_NORMAL_HIERARCHY_HPP_
#define BAYESMIX_HIERARCHIES_LIN_DEP_NORMAL_HIERARCHY_HPP_

#include <google/protobuf/stubs/casts.h>

#include <Eigen/Dense>
#include <cassert>
#include <memory>

#include "../../proto/cpp/hierarchy_prior.pb.h"
#include "../../proto/cpp/marginal_state.pb.h"
#include "base_hierarchy.hpp"
#include "dependent_hierarchy.hpp"

class LinDepNormalHierarchy : public DependentHierarchy {
 public:
  struct State {
    Eigen::VectorXd parameters;
  };
  struct Hyperparams {};  // TODO

 protected:
  double data_sum = 0.0;
  double data_sum_squares = 0.0;
  // STATE
  State state;
  // HYPERPARAMETERS
  std::shared_ptr<Hyperparams> hypers;
  // HYPERPRIOR
  std::shared_ptr<bayesmix::NNIGPrior> prior;  // TODO LDNormPrior?

  void clear_data() {
    data_sum = 0.0;
    data_sum_squares = 0.0;
    card = 0;
    cluster_data_idx.clear();
  }

  void update_summary_statistics(const Eigen::VectorXd &datum,
                                 bool add) override {
    if (add) {
      data_sum += datum(0);
      data_sum_squares += datum(0) * datum(0);
    } else {
      data_sum -= datum(0);
      data_sum_squares -= datum(0) * datum(0);
    }
  }

  // AUXILIARY TOOLS
  //! Returns updated values of the prior hyperparameters via their posterior
  Hyperparams some_update();  // TODO

 public:
  void initialize() override;
  //! Returns true if the hierarchy models multivariate data (here, false)
  bool is_multivariate() const override { return false; }

  void update_hypers(const std::vector<bayesmix::MarginalState::ClusterState>
                         &states) override;

  // DESTRUCTOR AND CONSTRUCTORS
  ~LinDepNormalHierarchy() = default;
  LinDepNormalHierarchy() = default;

  std::shared_ptr<BaseHierarchy> clone() const override {
    auto out = std::make_shared<LinDepNormalHierarchy>(*this);
    out->clear_data();
    return out;
  }

  // EVALUATION FUNCTIONS
  //! Evaluates the log-likelihood of data in a single point
  double like_lpdf(const Eigen::RowVectorXd &datum,
                   const Eigen::RowVectorXd &covariate) const override;
  //! Evaluates the log-likelihood of data in the given points
  Eigen::VectorXd like_lpdf_grid(
      const Eigen::MatrixXd &data,
      const Eigen::MatrixXd &covariates) const override;
  //! Evaluates the log-marginal distribution of data in a single point
  double marg_lpdf(const Eigen::RowVectorXd &datum,
                   const Eigen::RowVectorXd &covariate) const override;
  //! Evaluates the log-marginal distribution of data in the given points
  Eigen::VectorXd marg_lpdf_grid(
      const Eigen::MatrixXd &data,
      const Eigen::MatrixXd &covariates) const override;

  // SAMPLING FUNCTIONS
  //! Generates new values for state from the centering prior distribution
  void draw() override;
  //! Generates new values for state from the centering posterior distribution
  void sample_given_data() override;
  void sample_given_data(const Eigen::MatrixXd &data) override;

  // GETTERS AND SETTERS
  State get_state() const { return state; }
  Hyperparams get_hypers() const { return *hypers; }
  //! \param state_ State value to set
  //! \param check  If true, a state validity check occurs after assignment
  void set_state_from_proto(const google::protobuf::Message &state_) override;
  void set_prior(const google::protobuf::Message &prior_) override;
  void write_state_to_proto(google::protobuf::Message *out) const override;
  void write_hypers_to_proto(google::protobuf::Message *out) const override;

  std::string get_id() const override { return "LDNorm"; }
};

#endif  // BAYESMIX_HIERARCHIES_LIN_DEP_NORMAL_HIERARCHY_HPP_
