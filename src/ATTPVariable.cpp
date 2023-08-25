#include "ATTPVariable.hpp"
#include <iostream>

namespace ATTP
{
    namespace at = AnalysisTree;

    Variable::Variable(ATTPConfig::Var var_config) :
        in_name_{var_config.in_name},
        out_name_{var_config.out_name},
        type_{var_config.type}
    {
    }

    void Variable::AddField(at::BranchConfig& out_particles)
    {
        if(type_ == "int")
        {
            out_particles.AddField<int>(out_name_);
        }
        else if(type_ == "float")
        {
            out_particles.AddField<float>(out_name_);
        }
        else if(type_ == "bool")
        {
            out_particles.AddField<bool>(out_name_);
        }
        else
        {
            std::cout << "Unsupported type: " << type_ << std::endl;
            exit(1);
        }
    }

    void Variable::InitInId(const at::BranchConfig& in_branch_config)
    {
        in_id_ = in_branch_config.GetFieldId(in_name_);
    }

    void Variable::InitOutId(const at::BranchConfig& out_branch_config)
    {
        out_id_ = out_branch_config.GetFieldId(out_name_);
    }
    void Variable::Print()
    {
        using namespace std;
        cout << in_name_ << " : " << in_id_;
        cout << " -> ";
        cout << out_name_ << " : " << out_id_;
        cout << endl;
    }

}