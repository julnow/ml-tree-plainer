#include "MLTPConfigParser.hpp"

#include <boost/property_tree/json_parser.hpp>
#include <iostream>

namespace MLTPConfig
{
    namespace pt = boost::property_tree;
    Parser::Parser(std::string json_config_path) : json_config_path_(json_config_path)
    {
        pt::read_json(json_config_path_, root_);
    }
    std::vector<Branch> Parser::ParseBranches()
    {
        std::vector<Branch> branches;
        auto branches_root = root_.get_child("branches");
        for (pt::ptree::value_type &branch_tree : branches_root)
        {
            std::string branch_name = branch_tree.first;
            auto vars_root = branch_tree.second;
            auto branch = ParseBranch(vars_root, branch_name);
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

    void Parser::Print()
    {
        auto branches = ParseBranches();
        for (auto &branch : branches)
        {
            std::cout << "Branch name: " << branch.name << std::endl;
            std::cout << "Branch type: " << branch.type << std::endl;
            for (auto &var : branch.vars)
            {
                std::cout << '\t' << "var in name: " << var.in_name << std::endl;
                std::cout << '\t' << "var out name: " << var.out_name << std::endl;
                std::cout << '\t' << "var type: " << var.type << std::endl;
            }
        }

        std::cout << "input file list: " << ParseInputFileList() << std::endl;
        std::cout << "output file name: " << ParseOutputFileName() << std::endl;
        std::cout << "output branch name: " << ParseOutputBranchName() << std::endl;
    }

    Branch Parser::ParseBranch(boost::property_tree::ptree branch_root, std::string branch_name)
    {
        return {branch_name, "a", {ParseVar(root_)}};
    }

    Var Parser::ParseVar(boost::property_tree::ptree root) { return {"a", "a", "a"}; }

    void Parser::ValidateVar(Var &var)
    {
        assert(true);
    }

    void Parser::ValidateBranch(Branch &branch)
    {
        assert(true);
    }

    // std::vector<BinConfig> ONNXConfigParser::ParseBinConfigs()
    // {
    //     std::vector<BinConfig> bin_configs;
    //     for (pt::ptree::value_type &model_dict_val : root_.get_child("model_paths"))
    //     {
    //         pt::ptree model_dict = model_dict_val.second;
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
    //     for (pt::ptree::value_type &feature_name : root_.get_child("model_features"))
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
