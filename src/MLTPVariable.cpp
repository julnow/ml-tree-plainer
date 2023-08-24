#include "MLTPVariable.hpp"
#include <iostream>

namespace MLTP
{
    Variable::Variable(MLTPConfig::Var var_config) : in_name_{var_config.in_name}, out_name_{var_config.out_name}, type_{var_config.type}
    {
    }

    void Variable::AddField(AnalysisTree::BranchConfig &out_particles)
    {
        if (type_ == "int")
        {
            out_particles.AddField<int>(out_name_);
        }
        else if (type_ == "float")
        {
            out_particles.AddField<float>(out_name_);
        }
        else if (type_ == "bool")
        {
            out_particles.AddField<bool>(out_name_);
        }
        else
        {
            std::cout << "Unsupported type: " << type_ << std::endl;
            exit(1);
        }
    }

    void Variable::SetField(AnalysisTree::Container &out_particle, const AnalysisTree::Container &in_particle)
    {
        if (type_ == "int")
        {
            auto value = in_particle.GetField<int>(in_id_);
            out_particle.SetField(value, out_id_);
        }
        else if (type_ == "float")
        {
            auto value = in_particle.GetField<float>(in_id_);
            out_particle.SetField(value, out_id_);
        }
        else if (type_ == "bool")
        {
            auto value = in_particle.GetField<bool>(in_id_);
            out_particle.SetField(value, out_id_);
        }
        else
        {
            std::cout << "Unsupported type: " << type_ << std::endl;
            exit(1);
        }
    }

    void Variable::InitInId(const AnalysisTree::BranchConfig &in_branch_config)
    {
        in_id_ = in_branch_config.GetFieldId(in_name_);
    }

    void Variable::InitOutId(const AnalysisTree::BranchConfig &out_branch_config)
    {
        out_id_ = out_branch_config.GetFieldId(out_name_);
    }

}