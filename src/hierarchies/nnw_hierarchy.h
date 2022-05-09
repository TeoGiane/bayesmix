#ifndef BAYESMIX_HIERARCHIES_NNW_HIERARCHY_H_
#define BAYESMIX_HIERARCHIES_NNW_HIERARCHY_H_

#include "base_hierarchy.h"
#include "hierarchy_id.pb.h"
#include "likelihoods/multi_norm_likelihood.h"
#include "priors/nw_prior_model.h"
#include "src/utils/distributions.h"
#include "updaters/nnw_updater.h"

class NNWHierarchy
    : public BaseHierarchy<NNWHierarchy, MultiNormLikelihood, NWPriorModel> {
 public:
  NNWHierarchy() = default;
  ~NNWHierarchy() = default;

  bayesmix::HierarchyId get_id() const override {
    return bayesmix::HierarchyId::NNW;
  }

  void set_default_updater() { updater = std::make_shared<NNWUpdater>(); }

  void initialize_state() override {
    // Initialize likelihood dimension to prior one
    like->set_dim(prior->get_dim());
    // Get hypers and data dimension
    auto hypers = prior->get_hypers();
    unsigned int dim = like->get_dim();
    // Initialize likelihood state
    State::MultiLS state;
    state.mean = hypers.mean;
    prior->write_prec_to_state(
        hypers.var_scaling * Eigen::MatrixXd::Identity(dim, dim), &state);
    like->set_state(state);
  };

  double marg_lpdf(ProtoHypersPtr hier_params,
                   const Eigen::RowVectorXd &datum) const override {
    HyperParams pred_params = get_predictive_t_parameters(hier_params);
    Eigen::VectorXd diag = pred_params.scale_chol.diagonal();
    double logdet = 2 * log(diag.array()).sum();
    return bayesmix::multi_student_t_invscale_lpdf(
        datum, pred_params.deg_free, pred_params.mean, pred_params.scale_chol,
        logdet);
  }

  HyperParams get_predictive_t_parameters(ProtoHypersPtr hier_params) const {
    auto params = hier_params->nnw_state();
    // Compute dof and scale of marginal distribution
    unsigned int dim = like->get_dim();
    double nu_n = params.deg_free() - dim + 1;
    double coeff = (params.var_scaling() + 1) / (params.var_scaling() * nu_n);
    Eigen::MatrixXd scale_chol =
        Eigen::LLT<Eigen::MatrixXd>(bayesmix::to_eigen(params.scale()))
            .matrixU();
    Eigen::MatrixXd scale_chol_n = scale_chol / std::sqrt(coeff);
    // Return predictive t parameters
    HyperParams out;
    out.mean = bayesmix::to_eigen(params.mean());
    out.deg_free = nu_n;
    out.scale_chol = scale_chol_n;
    return out;
  }
};

#endif  // BAYESMIX_HIERARCHIES_NNW_HIERARCHY_H_
