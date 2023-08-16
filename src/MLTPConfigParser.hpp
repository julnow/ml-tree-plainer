#pragma once

#include <vector>
#include <string>

#include <boost/property_tree/ptree.hpp>

namespace MLTPConfig
{
    struct Var
    {
        std::string in_name;
        std::string out_name;
        std::string type;
    };
    struct Branch
    {
        std::string name;
        std::string type;
        std::vector<Var> vars;
    };

    class Parser
    {
    public:
        Parser(std::string json_config_path);

        std::vector<Branch> ParseBranches();
        std::string ParseInputFileList();
        std::string ParseOutputFileName();
        std::string ParseOutputBranchName();

        void Print();

    private:
        Branch ParseBranch(boost::property_tree::ptree branch_root, std::string branch_name);
        Var ParseVar(boost::property_tree::ptree root);

        void ValidateVar(Var &var);
        void ValidateBranch(Branch &branch);

        boost::property_tree::ptree root_;
        std::string json_config_path_;
    };
}