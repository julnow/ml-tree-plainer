#pragma once
#include "AnalysisTree/TaskManager.hpp"
#include "MLTPVariable.hpp"

namespace MLTP
{
    class Branch
    {
    public:
        Branch(
            MLTPConfig::Branch branch_config,
            const AnalysisTree::Configuration* atree_config,
            AnalysisTree::BranchConfig& out_branch_config);

        void AddFields();

        template <typename T>
        void SetFields(AnalysisTree::Particle& out_particle, const T& in_particle)
        {
            for(auto& v: vars_)
            {
                v.SetField<T>(out_particle, in_particle);
            }
        }

        void InitInIndices();
        void InitOutIndices();
        void Print();

    private:
        static const AnalysisTree::BranchConfig&
            ValidateBranch(std::string in_branch_name, const AnalysisTree::Configuration* config);
        static bool IsInBranchNameValid(std::string in_branch_name);
        std::vector<Variable> vars_;
        const AnalysisTree::BranchConfig& in_branch_config_;
        const AnalysisTree::BranchConfig& out_branch_config_;
    };
}