#include <iostream>

#include "gammagamma_hierarchy.h"
#include "src/includes.h"

Eigen::MatrixXd simulate_data(const unsigned int ndata) {
  Eigen::MatrixXd data(ndata, 1);
  auto& rng = bayesmix::Rng::Instance().get();
  for (int i = 0; i < ndata; i++) {
    if (stan::math::uniform_rng(0, 1, rng) < 0.5) {
      data(i, 0) = stan::math::gamma_rng(1, 5, rng);
    } else {
      data(i, 0) = stan::math::gamma_rng(1, 0.5, rng);
    }
  }
  return data;
}

int main() {
  // Simulate data
  Eigen::MatrixXd data = simulate_data(50);

  // Set up hierarchy
  bayesmix::GammaGammaPrior hier_prior;
  hier_prior.mutable_fixed_values()->set_shape(1.0);
  hier_prior.mutable_fixed_values()->set_rate_alpha(2.0);
  hier_prior.mutable_fixed_values()->set_rate_beta(2.0);
  auto hier = std::make_shared<GammaGammaHierarchy>();
  hier->get_mutable_prior()->CopyFrom(hier_prior);
  hier->initialize();

  // Set up mixing
  bayesmix::DPPrior mix_prior;
  mix_prior.mutable_fixed_value()->set_totalmass(1.0);
  auto mixing = MixingFactory::Instance().create_object("DP");
  mixing->get_mutable_prior()->CopyFrom(mix_prior);
  mixing->set_num_components(5);

  // Set up algorithm
  auto algo = AlgorithmFactory::Instance().create_object("Neal8");
  algo->set_mixing(mixing);
  algo->set_data(data);
  algo->set_hierarchy(hier);

  // Specify algorithm params
  bayesmix::AlgorithmParams params;
  params.set_algo_id("Neal8");
  params.set_rng_seed(0);
  params.set_burnin(1000);
  params.set_iterations(2000);
  params.set_init_num_clusters(10);
  params.set_neal8_n_aux(1);
  algo->read_params_from_proto(params);

  // Set up collector
  MemoryCollector* coll = new MemoryCollector();

  // Run algorithm
  algo->run(coll);

  delete coll;
  return 0;
}
