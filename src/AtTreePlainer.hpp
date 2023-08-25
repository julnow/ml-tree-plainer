#pragma once

#include "AnalysisTree/Cuts.hpp"
#include "AnalysisTree/Detector.hpp"
#include "AnalysisTree/EventHeader.hpp"
#include "AnalysisTree/Task.hpp"

#include "ATTPBranch.hpp"

class AtTreePlainer : public AnalysisTree::Task
{
public:
    explicit AtTreePlainer() = default;
    ~AtTreePlainer() override = default;

    void Init() override;
    void Exec() override;
    void Finish() override{};

    void InitIndices();

    void SetConfig(ATTPConfig::Config config)
    {
        json_config_ = config;
    }

protected:
    ATTPConfig::Branch FindBranch(std::string branch_name);

    AnalysisTree::Particles* simulated_{nullptr};
    AnalysisTree::Detector<AnalysisTree::Hit>* tofhits_{nullptr};
    AnalysisTree::Detector<AnalysisTree::Track>* vtxtracks_{nullptr};
    AnalysisTree::EventHeader* rec_event_header_{nullptr};
    AnalysisTree::Detector<AnalysisTree::Track>* trdtracks_{nullptr};

    AnalysisTree::Matching* tof2sim_match_{nullptr};
    AnalysisTree::Matching* vtx2tof_match_{nullptr};
    AnalysisTree::Matching* vtx2sim_match_{nullptr};
    AnalysisTree::Matching* vtx2trd_match_{nullptr};

    AnalysisTree::Particles* plain_branch_{nullptr};

    ATTP::Branch* event_header_;
    ATTP::Branch* tof_;
    ATTP::Branch* vtx_;
    ATTP::Branch* sim_;
    ATTP::Branch* trd_;

    ATTPConfig::Config json_config_;
};
