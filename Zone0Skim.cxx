#include "Framework/EventProcessor.h"

#include "SimCore/Event/SimTrackerHit.h"
#include "DetDescr/SimSpecialID.h"
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

class Zone0Skim : public framework::Analyzer {
 public:
  Zone0Skim(const std::string& name, framework::Process& p)
    : framework::Analyzer(name, p) {}
  ~Zone0Skim() override = default;
  void analyze(const framework::Event& event) override;
};

void Zone0Skim::analyze(const framework::Event& event) {
  // copy simulation momentum into vector and convert to GeV
  double energy_estimate{0};
  std::set<int> used_ids;
  const auto& ecal_sp_hits{event.getCollection<ldmx::SimTrackerHit>("EcalScoringPlaneHits", "")};
  for (const auto& hit : ecal_sp_hits) {
    auto pdg{hit.getPdgID()};
    if ((abs(pdg)==11) or pdg == 22) {
      // EM
      const auto& pos = hit.getPosition();
      bool in_impact_region = (
          (pos[0] >= -20 and pos[0] <= 20) and
          (pos[1] >= -50 and pos[1] <= 50)
      );
      if (in_impact_region and used_ids.find(hit.getTrackID()) == used_ids.end()) {
        used_ids.insert(hit.getTrackID());
        energy_estimate += (hit.getEnergy()/1000);
      }
    }
  }

  ldmx_log(info) << "energy estimate: " << energy_estimate << "GeV";

  const auto& unsorted_tracks{event.getCollection<ldmx::Track>("RecoilTracksClean", "")};
  auto tracks{sortByMomentum(unsorted_tracks)};
  if (tracks.size() == 0) {
    ldmx_log(error) << "no tracks in this event";
    return;
  }

  const auto& leadtrk{*(tracks[0])};
  auto leadtrk_momentum = mag(leadtrk.getMomentum());

  ldmx_log(info) << "lead track momentum: " << leadtrk_momentum << "GeV";

  if (energy_estimate < 3 and leadtrk_momentum > 6) {
    setStorageHint(framework::hint_shouldKeep);
  }
}

DECLARE_ANALYZER(Zone0Skim);
