#pragma once

#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>

namespace ATTPConfig
{
    using namespace boost::property_tree;

    struct Var
    {
        std::string in_name;
        std::string out_name;
        std::string type;
    };
    struct Branch
    {
        std::string name;
        std::vector<Var> vars;
    };

    struct Config
    {
        std::string input_filelist;
        std::string output_filename;
        std::string output_branch_name;
        std::vector<Branch> branches;
    };

    class Parser
    {
    public:
        static Config Parse(const std::string json_config_path);
        static void Print(const Config& config);

    private:
        Parser(std::string json_config_path);

        std::vector<Branch> ParseBranches();
        Branch ParseBranch(ptree branch_root, std::string branch_name);
        std::vector<Var> ParseVars(ptree vars_root);
        Var ParseVar(ptree var_root);

        std::string ParseInputFileList();
        std::string ParseOutputFileName();
        std::string ParseOutputBranchName();

        static void ValidateBranch(const Branch& branch);
        static void ValidateVar(const Var& var);

        const std::string json_config_path_;
        ptree root_;
    };
}