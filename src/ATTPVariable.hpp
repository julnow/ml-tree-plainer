#pragma once
#include <string>
#include <vector>
#include "ATTPConfigParser.hpp"
#include "AnalysisTree/Detector.hpp"
#include "AnalysisTree/EventHeader.hpp"
#include "AnalysisTree/Task.hpp"

namespace ATTP
{
    class Variable
    {
    public:
        Variable(ATTPConfig::Var var_config);

        void AddField(AnalysisTree::BranchConfig& out_particles);

        template <typename T> void SetField(AnalysisTree::Particle& out, const T& in)
        {
            if(type_ == "int")
            {
                auto value = in.template GetField<int>(in_id_);
                out.SetField(value, out_id_);
            }
            else if(type_ == "float")
            {
                auto value = in.template GetField<float>(in_id_);
                out.SetField(value, out_id_);
            }
            else if(type_ == "bool")
            {
                auto value = in.template GetField<bool>(in_id_);
                out.SetField(value, out_id_);
            }
            else
            {
                std::cout << "Unsupported type: " << type_ << std::endl;
                exit(1);
            }
        }

        void InitInId(const AnalysisTree::BranchConfig& in_branch_config);
        void InitOutId(const AnalysisTree::BranchConfig& out_branch_config);
        void Print();

    private:
        std::string in_name_;
        std::string out_name_;
        std::string type_;
        int in_id_ = 0;
        int out_id_ = 0;
    };

}