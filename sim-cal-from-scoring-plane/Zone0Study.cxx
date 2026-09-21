#include "Framework/EventProcessor.h"

#include "SimCore/Event/SimParticle.h"
#include "Ecal/Event/EcalHit.h"
#include "Hcal/Event/HcalHit.h"

class Zone0Study : public framework::Analyzer {
  int mindex_;
  std::set<int> seen_seed;
 public:
  Zone0Study(const std::string& name, framework::Process& p)
    : framework::Analyzer(name, p) {}
  ~Zone0Study() override = default;
  void onProcessStart() override {
    getHistoDirectory();
    static const char* tree_name = "z0ntuple";
    ntuple_.create(tree_name);
    ntuple_.addVar<int>(tree_name, "run");
    ntuple_.addVar<int>(tree_name, "event");
    ntuple_.addVar<int>(tree_name, "mindex");
    ntuple_.addVar<double>(tree_name, "ecal_total_rec_energy");
    ntuple_.addVar<double>(tree_name, "ecal_x");
    ntuple_.addVar<double>(tree_name, "ecal_y");
    ntuple_.addVar<double>(tree_name, "ecal_hit_rms");
    ntuple_.addVar<float>(tree_name, "hcal_max_pe");
    ntuple_.addVar<int>(tree_name, "hcal_n_bars_above_half_mip");
  }
  void onNewRun(const ldmx::RunHeader& header) {
    mindex_ = header.getIntParameter("MultiSimIndex");
  }
  void print_particle(std::string prefix, int current_track_id, const ldmx::SimParticle& current, const std::map<int, ldmx::SimParticle>& particles) {
    const auto& vtx = current.getVertex();
    printf("%s%d (%d, %f, [%f, %f, %f]) ->\n",
        prefix.c_str(), current_track_id, current.getPdgID(), current.getEnergy(),
        vtx[0], vtx[1], vtx[2]);
    for (int track_id : current.getDaughters()) {
      prefix += "  ";
      // skip low energy or unsaved children
      if (particles.find(track_id) != particles.end()) {
        const auto& child{particles.at(track_id)};
        if (child.getEnergy() < 50) continue;
        print_particle(prefix, track_id, child, particles);
      }
    }
  }
  void analyze(const framework::Event& event) override {
    int run = event.getEventHeader().getRun();
    int event_number = event.getEventNumber();
    // mindex_ still has its value from the last run that was started
    
    // luckily, the zone0 events are all in different runs
    // so just using the run number as a proxy for the unique seed event (run, event)
    if (seen_seed.find(run) == seen_seed.end()) {
      // haven't seen this seed event yet, do deep dive into seed
      seen_seed.insert(run);
      printf("Seed Event ID: (%d, %d)\n", run, event_number);
      printf("Seed Sim Particles (E > 50MeV):\n");
      const auto& sim_particles{event.getMap<int, ldmx::SimParticle>("SimParticles", "simtrack")};
      print_particle("", 1, sim_particles.at(1), sim_particles);

      printf("CalSim Primaries:\n");
      const auto& simcal_particles{event.getMap<int, ldmx::SimParticle>("SimParticles", "simcal")};
      double total_primary_energy{0};
      for (const auto& [tid, particle] : simcal_particles) {
        if (particle.getGenStatus() != 1) continue;
        const auto& vtx = particle.getVertex();
        const auto& p = particle.getMomentum();
        total_primary_energy += particle.getEnergy();
        printf("%d (pdg %d, energy %f, vtx [%f, %f, %f], p [%f, %f, %f])\n",
              tid, particle.getPdgID(), particle.getEnergy(),
              vtx[0], vtx[1], vtx[2],
              p[0], p[1], p[2]);
      }
      printf("Total Primary Energy: %f\n", total_primary_energy);
    }

    const auto& ecal_rec_hits{event.getCollection<ldmx::EcalHit>("EcalRecHits", "")};
    double ecal_total_rec_energy{0.};
    double ecal_x{0.}, ecal_y{0.};
    double ecal_hit_rms{0.};
    for (const auto& hit : ecal_rec_hits) {
      ecal_total_rec_energy += hit.getEnergy();
      ecal_x += hit.getEnergy()*hit.getXPos();
      ecal_y += hit.getEnergy()*hit.getYPos();
    }
    if (ecal_total_rec_energy > 0.0) {
      ecal_x /= ecal_total_rec_energy;
      ecal_y /= ecal_total_rec_energy;
      for (const auto& hit : ecal_rec_hits) {
        double dx = hit.getXPos() - ecal_x;
        double dy = hit.getYPos() - ecal_y;
        ecal_hit_rms += hit.getEnergy()*sqrt(dx*dx + dy*dy);
      }
      ecal_hit_rms /= ecal_total_rec_energy;
    }

    const auto& hcal_rec_hits{event.getCollection<ldmx::HcalHit>("HcalRecHits", "")};
    float hcal_max_pe{0.};
    int hcal_n_bars_above_half_mip{0};
    for (const auto& hit : hcal_rec_hits) {
      if (hit.getPE() > hcal_max_pe) hcal_max_pe = hit.getPE();
      if (hit.getPE() > 0.5*70) hcal_n_bars_above_half_mip++;
    }

    ntuple_.setVar("run", run);
    ntuple_.setVar("event", event_number);
    ntuple_.setVar("mindex", mindex_);
    ntuple_.setVar("ecal_total_rec_energy", ecal_total_rec_energy);
    ntuple_.setVar("ecal_x", ecal_x);
    ntuple_.setVar("ecal_y", ecal_y);
    ntuple_.setVar("ecal_hit_rms", ecal_hit_rms);
    ntuple_.setVar("hcal_max_pe", hcal_max_pe);
    ntuple_.setVar("hcal_n_bars_above_half_mip", hcal_n_bars_above_half_mip);
  }
};

DECLARE_ANALYZER(Zone0Study);
