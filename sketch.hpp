#include <iostream>
#include <vector>

// TODO: HOOK UP LIBBOOST::JSON

class ConfigParser
{
public:
    ConfigParser(std::string path_to_config);
    Config Parse();
};

class ConfigVariable
{
public:
    ConfigVariable(std::string inname, std::string outName, std::string type);

private:
    enum VarTypes
    {
        FLOAT,
        INT
    };
    std::string _inName;
    std::string _outName;
    VarTypes type;
};

class ConfigBranch
{

private:
    std::string _branchName;
    std::vector<ConfigVariable> _vars;
    std::string _type;
};

class Config
{
public:
    Config() = delete;
    friend ConfigParser;

private:
    std::vector<ConfigBranch> branches;
    std::string _inputFileName;
    std::string _outputFileName;
    std::string _outputBranchName;
};

class BranchPlainer
{
public:
    BranchPlainer(TFile filein, TFile fileout, ConfigBranch branchConfig);
    void Plain();
};

class Plainer
{
public:
    Plainer(Config config);
    std::vector<BranchPlainer> _branchPlainers;
};