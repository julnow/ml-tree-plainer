#pragma once
#include <string>
#include <vector>
#include "MLTPConfigParser.hpp"
#include "AnalysisTree/Detector.hpp"
#include "AnalysisTree/EventHeader.hpp"
#include "AnalysisTree/Task.hpp"

namespace MLTP
{
    class Variable
    {
    public:
        Variable(MLTPConfig::Var var_config);

        void AddField(AnalysisTree::BranchConfig &out_particles);
        void SetField(AnalysisTree::Container &out_particle, const AnalysisTree::Container &in_particle);
        void InitInId(const AnalysisTree::BranchConfig &in_branch_config);
        void InitOutId(const AnalysisTree::BranchConfig &out_branch_config);

    private:
        std::string in_name_;
        std::string out_name_;
        std::string type_;
        int in_id_ = 0;
        int out_id_ = 0;
    };

}