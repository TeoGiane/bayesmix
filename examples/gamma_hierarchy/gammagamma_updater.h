#ifndef BAYESMIX_HIERARCHIES_GAMMA_GAMMA_UPDATER_H_
#define BAYESMIX_HIERARCHIES_GAMMA_GAMMA_UPDATER_H_

#include "gamma_likelihood.h"
#include "gamma_prior_model.h"
#include "src/hierarchies/updaters/semi_conjugate_updater.h"

class GammaGammaUpdater
    : public SemiConjugateUpdater<GammaLikelihood, GammaPriorModel> {
 public:
  GammaGammaUpdater() = default;
  ~GammaGammaUpdater() = default;
  bool is_conjugate() const override { return true; };
  ProtoHypersPtr compute_posterior_hypers(AbstractLikelihood& like,
                                          AbstractPriorModel& prior) override;
  std::shared_ptr<AbstractUpdater> clone() const override;
};

/* DEFINITIONS */
AbstractUpdater::ProtoHypersPtr GammaGammaUpdater::compute_posterior_hypers(
    AbstractLikelihood& like, AbstractPriorModel& prior) {
  // Likelihood and Prior downcast
  auto& likecast = downcast_likelihood(like);
  auto& priorcast = downcast_prior(prior);
  // Getting required quantities from likelihood and prior
  int card = likecast.get_card();
  double data_sum = likecast.get_data_sum();
  auto hypers = priorcast.get_hypers();
  // No update possible
  if (card == 0) {
    return priorcast.get_hypers_proto();
  }
  // Compute posterior hyperparameters
  double rate_alpha_new = hypers.rate_alpha + hypers.shape * card;
  double rate_beta_new = hypers.rate_beta + data_sum;
  // Proto conversion
  ProtoHypers out;
  bayesmix::GammaGammaDistribution unpacked_out;
  unpacked_out.set_shape(hypers.shape);
  unpacked_out.set_rate_alpha(rate_alpha_new);
  unpacked_out.set_rate_beta(rate_beta_new);
  out.mutable_custom_state()->PackFrom(unpacked_out);
  return std::make_shared<ProtoHypers>(out);
}

std::shared_ptr<AbstractUpdater> GammaGammaUpdater::clone() const {
  auto out = std::make_shared<GammaGammaUpdater>(
      static_cast<GammaGammaUpdater const&>(*this));
  out->clear_hypers();
  return out;
}

#endif  // BAYESMIX_HIERARCHIES_GAMMA_GAMMA_UPDATER_H_
