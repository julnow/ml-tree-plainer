#pragma once

#include "AnalysisTree/Cuts.hpp"
#include "AnalysisTree/Detector.hpp"
#include "AnalysisTree/EventHeader.hpp"
#include "AnalysisTree/Task.hpp"

#include "MLTPBranch.hpp"

class MlTreePlainer : public AnalysisTree::Task
{
public:
  explicit MlTreePlainer() = default;
  ~MlTreePlainer() override = default;

  void Init() override;
  void Exec() override;
  void Finish() override{};

  void InitIndices();
  void SetConfig(MLTPConfig::Config config);

  // void SetCuts(AnalysisTree::Cuts* cuts) { cuts_ = cuts; }

protected:
  MLTPConfig::Branch FindBranch(std::string branch_name);

  // input branches
  AnalysisTree::Particles *simulated_{nullptr};
  AnalysisTree::Detector<AnalysisTree::Hit> *tofhits_{nullptr};
  AnalysisTree::Detector<AnalysisTree::Track> *vtxtracks_{nullptr};
  AnalysisTree::EventHeader *rec_event_header_{nullptr};
  AnalysisTree::Detector<AnalysisTree::Track> *trdtracks_{nullptr};

  AnalysisTree::Matching *tof2sim_match_{nullptr};
  AnalysisTree::Matching *vtx2tof_match_{nullptr};
  AnalysisTree::Matching *vtx2sim_match_{nullptr};
  AnalysisTree::Matching *vtx2trd_match_{nullptr};

  // output branch
  AnalysisTree::Particles *plain_branch_{nullptr};

  //**** input fields ***********
  MLTP::Branch *event_header_;
  MLTP::Branch *tof_;
  MLTP::Branch *vtx_;
  MLTP::Branch *sim_;
  MLTP::Branch *trd_;

  MLTPConfig::Config json_config_;
};
