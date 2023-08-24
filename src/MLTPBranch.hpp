#pragma once
#include "MLTPVariable.hpp"
#include "AnalysisTree/TaskManager.hpp"

namespace MLTP
{
    class Branch
    {
    public:
        Branch(MLTPConfig::Branch branch_config, const AnalysisTree::Configuration *atree_config, AnalysisTree::BranchConfig &out_branch_config);

        void AddFields();
        void SetFields(AnalysisTree::Container &out_particle, const AnalysisTree::Container &in_particle);
        void InitInIndices();
        void InitOutIndices();

    private:
        static const AnalysisTree::BranchConfig &ValidateBranch(std::string in_branch_name, const AnalysisTree::Configuration *config);
        static bool IsInBranchNameValid(std::string in_branch_name);
        std::vector<Variable> vars_;
        const AnalysisTree::BranchConfig &in_branch_config_;
        const AnalysisTree::BranchConfig &out_branch_config_;
    };
}