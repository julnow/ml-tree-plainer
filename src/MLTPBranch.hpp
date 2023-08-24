#pragma once
#include "MLTPVariable.hpp"
#include "AnalysisTree/TaskManager.hpp"

namespace MLTP
{
    class Branch
    {
    public:
        Branch(MLTPConfig::Branch branch_config, AnalysisTree::Configuration *atree_config, AnalysisTree::BranchConfig &out_branch_config);

        void AddFields();
        void SetFields(AnalysisTree::Container &out_particle, AnalysisTree::Container &in_particle);
        void InitInIndices();
        void InitOutIndices();

    private:
        static AnalysisTree::BranchConfig &ValidateBranch(std::string in_branch_name, AnalysisTree::Configuration *config);
        static bool IsInBranchNameValid(std::string in_branch_name);
        std::vector<Variable> vars_;
        AnalysisTree::BranchConfig &in_branch_config_;
        AnalysisTree::BranchConfig &out_branch_config_;
    };
}