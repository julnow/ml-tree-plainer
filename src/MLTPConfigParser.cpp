#include "MLTPConfigParser.hpp"

#include <boost/property_tree/json_parser.hpp>
#include <iostream>

namespace MLTPConfig
{
    using namespace boost::property_tree;

    Config Parser::Parse(std::string json_config_path)
    {
        Parser parser{json_config_path};
        auto branches = parser.ParseBranches();
        auto input_filelist = parser.ParseInputFileList();
        auto output_filename = parser.ParseOutputFileName();
        auto output_branch_name = parser.ParseOutputBranchName();
        return {input_filelist, output_filename, output_branch_name, branches};
    }

    void Parser::Print(const Config &config)
    {
        using namespace std;
        cout << "CONFIG:" << '\n';
        cout << "input file list: " << config.input_filelist << '\n';
        cout << "output file name: " << config.output_filename << '\n';
        cout << "output branch name: " << config.output_branch_name << '\n';
        cout << "Branches: " << config.output_branch_name << '\n';
        for (auto &branch : config.branches)
        {
            cout << "Branch name: " << branch.name << '\n';
            cout << "Branch type: " << branch.type << '\n';
            for (auto &var : branch.vars)
            {
                cout << '\t' << "var in name: " << var.in_name << '\n';
                cout << '\t' << "var out name: " << var.out_name << '\n';
                cout << '\t' << "var type: " << var.type << '\n';
            }
        }
        cout << endl;
    }

    Parser::Parser(const std::string json_config_path) : json_config_path_(json_config_path)
    {
        read_json(json_config_path_, root_);
    }

    std::vector<Branch> Parser::ParseBranches()
    {
        std::vector<Branch> branches;
        auto branches_root = root_.get_child("branches");
        for (ptree::value_type &branch_tree : branches_root)
        {
            std::string branch_name = branch_tree.first;
            auto branch_root = branch_tree.second;
            auto branch = ParseBranch(branch_root, branch_name);
            branches.push_back(branch);
        }
        return branches;
    }

    std::string Parser::ParseInputFileList()
    {
        return root_.get<std::string>("inputFileList");
    }

    std::string Parser::ParseOutputFileName()
    {
        return root_.get<std::string>("outputFileName");
    }

    std::string Parser::ParseOutputBranchName()
    {
        return root_.get<std::string>("outputBranchName");
    }

    Branch Parser::ParseBranch(const ptree branch_root, std::string branch_name)
    {
        std::string branch_type = branch_root.get<std::string>("type");
        auto vars_root = branch_root.get_child("vars");
        std::vector<Var> vars = ParseVars(vars_root);

        Branch branch = {branch_name, branch_type, vars};
        ValidateBranch(branch);
        return branch;
    }

    std::vector<Var> Parser::ParseVars(ptree vars_root)
    {
        std::vector<Var> vars;
        for (ptree::value_type &var_tree : vars_root)
        {
            auto var_root = var_tree.second;
            Var var = ParseVar(var_root);
            vars.push_back(var);
        }
        return vars;
    }

    Var Parser::ParseVar(ptree var_root)
    {
        std::string in_name = var_root.get<std::string>("inName");
        std::string out_name = var_root.get<std::string>("outName");
        std::string type = var_root.get<std::string>("type");
        Var var = {in_name, out_name, type};
        ValidateVar(var);
        return var;
    }

    void Parser::ValidateBranch(const Branch &branch)
    {
        assert(true);
    }

    void Parser::ValidateVar(const Var &var)
    {
        assert(true);
    }

    // std::vector<BinConfig> ONNXConfigParser::ParseBinConfigs()
    // {
    //     std::vector<BinConfig> bin_configs;
    //     for (ptree::value_type &model_dict_val : root_.get_child("model_paths"))
    //     {
    //         ptree model_dict = model_dict_val.second;
    //         float min = model_dict.get<float>("lo");
    //         float max = model_dict.get<float>("hi");
    //         std::string path = model_dict.get<std::string>("path");

    //         BinConfig bin_config{min, max, path};
    //         bin_configs.push_back(bin_config);
    //     }
    //     std::cout << "Loaded ranges and paths from " << json_config_path_ << ':' << std::endl;
    //     for (auto &config : bin_configs)
    //     {
    //         std::cout << '(' << config.min << " : " << config.max << ']' << " -> " << config.path << std::endl;
    //     }
    //     return bin_configs;
    // }

    // std::vector<std::string> ONNXConfigParser::ParseModelFeatures()
    // {
    //     std::vector<std::string> model_features;
    //     for (ptree::value_type &feature_name : root_.get_child("model_features"))
    //     {
    //         std::string model_feature = feature_name.second.data();
    //         model_features.push_back(model_feature);
    //     }
    //     std::cout << "Loaded feature variables " << json_config_path_ << ':' << std::endl;
    //     for (auto &feature : model_features)
    //     {
    //         std::cout << feature << std::endl;
    //     }
    //     return model_features;
    // }
}
