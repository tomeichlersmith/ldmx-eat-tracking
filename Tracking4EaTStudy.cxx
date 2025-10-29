#include "Framework/EventProcessor.h"

#include <functional>

#include "Tracking/Event/Track.h"

template<typename T>
T mag(const std::vector<T>& v) {
  T mag2{0};
  for (const auto& c: v) {
    mag2 += c*c;
  }
  return std::sqrt(mag2);
}

/**
 * Tracking coordinates are a rotation of detector coordinates
 * so that the B field is along z.
 *
 * Track | Det
 * ------|----
 * y     | x
 * z     | y
 * x     | z
 */
template<typename T>
std::vector<T> toLDMX(const std::vector<T>& v) {
  return {v[1], v[2], v[0]};
}

/**
 * perigee_pars_[0] -> d0 -> global X
 * perigee_pars_[1] -> z0 -> global Y
 *
 * returns (x,y) in detector coordinates
 * ```cpp
 * auto [x, y] = getImpactPoint(state);
 * ```
 */
std::tuple<double,double> getImpactPoint(const ldmx::Track::TrackState &state) {
  return std::make_tuple(state.params[0], state.params[1]);
}

/**
 * sort the input vector of tracks by momentum magnitude
 *
 * using pointers to avoid copying data
 */
std::vector<const ldmx::Track*> sortByMomentum(const std::vector<ldmx::Track>& tracks) {
  std::vector<const ldmx::Track*> sorted_tracks;
  sorted_tracks.reserve(tracks.size());
  for (const auto& track : tracks) { sorted_tracks.emplace_back(&track); }
  std::sort(
    sorted_tracks.begin(),
    sorted_tracks.end(),
    [](const ldmx::Track* lhs, const ldmx::Track* rhs) {
      return mag(lhs->getMomentum()) > mag(rhs->getMomentum());
    }
  );
  return sorted_tracks;
}

class Tracking4EaTStudy : public framework::Analyzer {
 public:
  Tracking4EaTStudy(const std::string& name, framework::Process& p)
    : framework::Analyzer(name, p) {}
  ~Tracking4EaTStudy() override = default;
  void onProcessStart() override;
  void analyze(const framework::Event& event) override;
};

void Tracking4EaTStudy::onProcessStart() {
  getHistoDirectory();
  histograms_.create("n_tracks", "N Tracks", 10, -0.5, 9.5);
  histograms_.create("n_clean_tracks", "N Clean Tracks", 10, -0.5, 9.5);
  histograms_.create("sub_leading_momentum", "Sub-Leading p [GeV]", 100, 0, 10);
  histograms_.create("track_state_at_ecal", "Has a ECal Track State", 2, -0.5, 1.5);
  histograms_.create("rec_momentum", "p [GeV]", 100, 0, 10);
  histograms_.create("sim_momentum", "p [GeV]", 100, 0, 10);
  histograms_.create("sim_momentum_no_clean_tracks", "p [GeV]", 100, 0, 10);
  histograms_.create("rec_momentum_sim_momentum",
      "Sim p [GeV]", 100, 0, 10,
      "Rec p [GeV]", 100, 0, 10);
  histograms_.create("impact_point",
      "X [mm]", 200, -200.0, 200.0,
      "Y [mm]", 200, -200.0, 200.0);
}

void Tracking4EaTStudy::analyze(const framework::Event& event) {
  const auto& clean_tracks{event.getCollection<ldmx::Track>("RecoilTracksClean", "")};
  histograms_.fill("n_clean_tracks", clean_tracks.size());

  // copy simulation momentum into vector and convert to GeV
  std::vector<double> sim_momentum = {
    event.getObject<double>("PEFFPx", "eat")/1000,
    event.getObject<double>("PEFFPy", "eat")/1000,
    event.getObject<double>("PEFFPz", "eat")/1000
  };
  auto sim_momentum_mag = mag(sim_momentum);

  if (clean_tracks.size() == 0) {
    histograms_.fill("sim_momentum_no_clean_tracks", sim_momentum_mag);
    return;
  }

  auto tracks{sortByMomentum(clean_tracks)};
  const auto& trk{*(tracks[0])};
  auto rec_momentum_mag = mag(trk.getMomentum());
  histograms_.fill("rec_momentum", rec_momentum_mag);
  histograms_.fill("sim_momentum", sim_momentum_mag);
  histograms_.fill("rec_momentum_sim_momentum", sim_momentum_mag, rec_momentum_mag);

  if (tracks.size() > 1) {
    histograms_.fill("sub_leading_momentum", mag(tracks[1]->getMomentum()));
  } else {
    histograms_.fill("sub_leading_momentum", 0);
  }

  auto track_at_ecal{trk.getTrackState(ldmx::TrackStateType::AtECAL)};
  histograms_.fill("track_state_at_ecal", track_at_ecal ? 1 : 0);
  if (not track_at_ecal) {
    return;
  }

  auto [x, y] = getImpactPoint(track_at_ecal.value());
  histograms_.fill("impact_point", x, y);

}

DECLARE_ANALYZER(Tracking4EaTStudy);
