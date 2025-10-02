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
  histograms_.create("track_state_at_ecal", "Has a ECal Track State", 2, -0.5, 1.5);
  histograms_.create("impact_point",
      "X [mm]", 200, -200.0, 200.0,
      "Y [mm]", 200, -200.0, 200.0);
}

void Tracking4EaTStudy::analyze(const framework::Event& event) {
  const auto& tracks_data{event.getCollection<ldmx::Track>("RecoilTracks", "")};
  const auto& clean_tracks{event.getCollection<ldmx::Track>("RecoilTracksClean", "")};
  histograms_.fill("n_tracks", tracks_data.size());
  histograms_.fill("n_clean_tracks", clean_tracks.size());

  for (const auto& trk: tracks_data) {
    auto track_at_ecal{trk.getTrackState(ldmx::TrackStateType::AtECAL)};
    histograms_.fill("track_state_at_ecal", track_at_ecal ? 1 : 0);
    if (not track_at_ecal) {
      continue;
    }
    auto [x, y] = getImpactPoint(track_at_ecal.value());
    histograms_.fill("impact_point", x, y);
  }

  std::vector<const ldmx::Track*> tracks;
  tracks.reserve(tracks_data.size());
  for (const auto& track : tracks_data) { tracks.emplace_back(&track); }
  std::sort(
    tracks.begin(),
    tracks.end(),
    [](const ldmx::Track* lhs, const ldmx::Track* rhs) {
      return mag(lhs->getMomentum()) > mag(rhs->getMomentum());
    }
  );
}

DECLARE_ANALYZER(Tracking4EaTStudy);
