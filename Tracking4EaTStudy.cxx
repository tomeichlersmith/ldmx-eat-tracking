#include "Framework/EventProcessor.h"

#include "Tracking/Event/Track.h"

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
  histograms_.create("ntracks", "N Tracks", 10, -0.5, 9.5);
}

void Tracking4EaTStudy::analyze(const framework::Event& event) {
  const auto& tracks{event.getCollection<ldmx::Track>("RecoilTracks", "")};
  histograms_.fill("ntracks", tracks.size());
}

DECLARE_ANALYZER(Tracking4EaTStudy);
