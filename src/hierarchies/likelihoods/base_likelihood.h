#ifndef BAYESMIX_HIERARCHIES_LIKELIHOODS_BASE_LIKELIHOOD_H_
#define BAYESMIX_HIERARCHIES_LIKELIHOODS_BASE_LIKELIHOOD_H_

#include <google/protobuf/message.h>

#include <memory>
#include <stan/math/rev.hpp>
// #include <random>
#include <set>
// #include <stan/math/prim.hpp>

#include "abstract_likelihood.h"
#include "algorithm_state.pb.h"

template <class Derived, typename State>
class BaseLikelihood : public AbstractLikelihood {
 public:
  BaseLikelihood() = default;
  ~BaseLikelihood() = default;

  virtual std::shared_ptr<AbstractLikelihood> clone() const override {
    auto out = std::make_shared<Derived>(static_cast<Derived const &>(*this));
    out->clear_data();
    out->clear_summary_statistics();
    return out;
  }

  // The unconstrained parameters are mean and log(var)
  double cluster_lpdf_from_unconstrained(
      Eigen::VectorXd unconstrained_params) const override {
    return static_cast<const Derived &>(*this)
        .template cluster_lpdf_from_unconstrained<double>(
            unconstrained_params);
  }

  stan::math::var cluster_lpdf_from_unconstrained(
      Eigen::Matrix<stan::math::var, Eigen::Dynamic, 1> unconstrained_params)
      const override {
    return static_cast<const Derived &>(*this)
        .template cluster_lpdf_from_unconstrained<stan::math::var>(
            unconstrained_params);
  }

  virtual Eigen::VectorXd lpdf_grid(const Eigen::MatrixXd &data,
                                    const Eigen::MatrixXd &covariates =
                                        Eigen::MatrixXd(0, 0)) const override;

  int get_card() const { return card; }

  double get_log_card() const { return log_card; }

  std::set<int> get_data_idx() const { return cluster_data_idx; }

  void write_state_to_proto(google::protobuf::Message *out) const override;

  State get_state() const { return state; }

  Eigen::VectorXd get_unconstrained_state() override {
    return state.get_unconstrained();
  }

  void set_state(const State &_state) { state = _state; };

  void set_state_from_unconstrained(
      const Eigen::VectorXd &unconstrained_state) override {
    state.set_from_unconstrained(unconstrained_state);
  }

  void add_datum(
      const int id, const Eigen::RowVectorXd &datum,
      const Eigen::RowVectorXd &covariate = Eigen::RowVectorXd(0)) override;

  void remove_datum(
      const int id, const Eigen::RowVectorXd &datum,
      const Eigen::RowVectorXd &covariate = Eigen::RowVectorXd(0)) override;

  void clear_data() {
    set_card(0);
    cluster_data_idx = std::set<int>();
  }

 protected:
  void set_card(const int card_) {
    card = card_;
    log_card = (card_ == 0) ? stan::math::NEGATIVE_INFTY : std::log(card_);
  }

  bayesmix::AlgorithmState::ClusterState *downcast_state(
      google::protobuf::Message *state_) const {
    return google::protobuf::internal::down_cast<
        bayesmix::AlgorithmState::ClusterState *>(state_);
  }

  const bayesmix::AlgorithmState::ClusterState &downcast_state(
      const google::protobuf::Message &state_) const {
    return google::protobuf::internal::down_cast<
        const bayesmix::AlgorithmState::ClusterState &>(state_);
  }

  State state;

  int card = 0;

  double log_card = stan::math::NEGATIVE_INFTY;

  std::set<int> cluster_data_idx;
};

template <class Derived, typename State>
void BaseLikelihood<Derived, State>::add_datum(
    const int id, const Eigen::RowVectorXd &datum,
    const Eigen::RowVectorXd &covariate) {
  assert(cluster_data_idx.find(id) == cluster_data_idx.end());
  card += 1;
  log_card = std::log(card);
  static_cast<Derived *>(this)->update_summary_statistics(datum, covariate,
                                                          true);
  cluster_data_idx.insert(id);
}

template <class Derived, typename State>
void BaseLikelihood<Derived, State>::remove_datum(
    const int id, const Eigen::RowVectorXd &datum,
    const Eigen::RowVectorXd &covariate) {
  static_cast<Derived *>(this)->update_summary_statistics(datum, covariate,
                                                          false);
  set_card(card - 1);
  auto it = cluster_data_idx.find(id);
  assert(it != cluster_data_idx.end());
  cluster_data_idx.erase(it);
}

template <class Derived, typename State>
void BaseLikelihood<Derived, State>::write_state_to_proto(
    google::protobuf::Message *out) const {
  std::shared_ptr<bayesmix::AlgorithmState::ClusterState> state_ =
      get_state_proto();
  auto *out_cast = downcast_state(out);
  out_cast->CopyFrom(*state_.get());
  out_cast->set_cardinality(card);
}

template <class Derived, typename State>
Eigen::VectorXd BaseLikelihood<Derived, State>::lpdf_grid(
    const Eigen::MatrixXd &data, const Eigen::MatrixXd &covariates) const {
  Eigen::VectorXd lpdf(data.rows());
  if (covariates.cols() == 0) {
    // Pass null value as covariate
    for (int i = 0; i < data.rows(); i++) {
      lpdf(i) = static_cast<Derived const *>(this)->lpdf(
          data.row(i), Eigen::RowVectorXd(0));
    }
  } else if (covariates.rows() == 1) {
    // Use unique covariate
    for (int i = 0; i < data.rows(); i++) {
      lpdf(i) = static_cast<Derived const *>(this)->lpdf(data.row(i),
                                                         covariates.row(0));
    }
  } else {
    // Use different covariates
    for (int i = 0; i < data.rows(); i++) {
      lpdf(i) = static_cast<Derived const *>(this)->lpdf(data.row(i),
                                                         covariates.row(i));
    }
  }
  return lpdf;
}

#endif  // BAYESMIX_HIERARCHIES_LIKELIHOODS_BASE_LIKELIHOOD_H_
