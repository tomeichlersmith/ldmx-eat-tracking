#include "Framework/EventProcessor.h"

class CheckMultiSimIndex : public framework::Analyzer {
  std::string instance_name_;
  int mindex_;
 public:
  CheckMultiSimIndex(const std::string& name, framework::Process& p)
    : framework::Analyzer(name, p), instance_name_{name} {}
  ~CheckMultiSimIndex() override = default;
  void onProcessStart() override {
    getHistoDirectory();
    ntuple_.create(instance_name_);
    ntuple_.addVar<int>(instance_name_, "run");
    ntuple_.addVar<int>(instance_name_, "event");
    ntuple_.addVar<int>(instance_name_, "mindex");
  }
  void onNewRun(const ldmx::RunHeader& header) {
    mindex_ = header.getIntParameter("MultiSimIndex");
  }
  void analyze(const framework::Event& event) override {
    int run = event.getEventHeader().getRun();
    int event_number = event.getEventNumber();
    // mindex_ still has its value from the last run that was started

    ntuple_.setVar("run", run);
    ntuple_.setVar("event", event_number);
    ntuple_.setVar("mindex", mindex_);
  }
};

DECLARE_ANALYZER(CheckMultiSimIndex);
