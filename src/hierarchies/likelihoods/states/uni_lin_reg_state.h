#ifndef BAYESMIX_HIERARCHIES_LIKELIHOODS_STATES_UNI_LIN_REG_STATE_H_
#define BAYESMIX_HIERARCHIES_LIKELIHOODS_STATES_UNI_LIN_REG_STATE_H_

#include <stan/math/prim.hpp>
#include <stan/math/rev.hpp>
#include <tuple>

#include "algorithm_state.pb.h"
#include "src/utils/proto_utils.h"

// TODO: CHECK VECTOR ASSIGNMENTS AND POSITIONING!
namespace State {

template <typename T>
Eigen::Matrix<T, Eigen::Dynamic, 1> uni_lin_reg_to_constrained(
    Eigen::Matrix<T, Eigen::Dynamic, 1> in) {
  int N = in.size();
  Eigen::Matrix<T, Eigen::Dynamic, 1> out(N);
  out << in.head(N - 1), stan::math::exp(in(N - 1));
  return out;
}

template <typename T>
Eigen::Matrix<T, Eigen::Dynamic, 1> uni_lin_reg_to_unconstrained(
    Eigen::Matrix<T, Eigen::Dynamic, 1> in) {
  int N = in.size();
  Eigen::Matrix<T, Eigen::Dynamic, 1> out(N);
  out << in.head(N - 1), stan::math::log(in(N - 1));
  return out;
}

template <typename T>
T uni_lin_reg_log_det_jac(Eigen::Matrix<T, Eigen::Dynamic, 1> constrained) {
  T out = 0;
  int N = constrained.size();
  stan::math::positive_constrain(stan::math::log(constrained(N - 1)), out);
  return out;
}

class UniLinReg {
 public:
  Eigen::VectorXd regression_coeffs;
  double var;

  Eigen::VectorXd get_unconstrained() {
    Eigen::VectorXd temp(regression_coeffs.size() + 1);
    temp << regression_coeffs, var;
    return uni_lin_reg_to_unconstrained(temp);
  }

  void set_from_unconstrained(Eigen::VectorXd in) {
    Eigen::VectorXd temp = uni_lin_reg_to_constrained(in);
    int dim = in.size() - 1;
    regression_coeffs = temp.head(dim);
    var = temp(dim);
  }

  void set_from_proto(const bayesmix::AlgorithmState::ClusterState &state_) {
    mean = state_.uni_ls_state().mean();
    var = state_.uni_ls_state().var();
  }

  // bayesmix::AlgorithmState::ClusterState get_as_proto() {
  //   bayesmix::AlgorithmState::ClusterState state;
  //   state.mutable_uni_ls_state()->set_mean(mean);
  //   state.mutable_uni_ls_state()->set_var(var);
  //   return state;
  // }

  double log_det_jac() {
    Eigen::VectorXd temp(regression_coeffs.size() + 1);
    temp << regression_coeffs, var;
    return uni_lin_reg_log_det_jac(temp);
  }
};

}  // namespace State

#endif  // BAYESMIX_HIERARCHIES_LIKELIHOODS_STATES_UNI_LIN_REG_STATE_H_
