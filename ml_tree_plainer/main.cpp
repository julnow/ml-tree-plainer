#include "ATreePlainer.hpp"

#include "AnalysisTree/TaskManager.hpp"
#include "AnalysisTree/PlainTreeFiller.hpp"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Wrong number of arguments! Please use:\n  ./main filelist.txt\n";
    return EXIT_FAILURE;
  }

  const bool make_plain_ttree{true};

  const std::string& filename = argv[1];

  auto* man = AnalysisTree::TaskManager::GetInstance();
  man->SetOutputName("intermediate_tree.root", "pTree");

  auto* at_plainer_task = new ATreePlainer();

  // AnalysisTree::Cuts* cuts = new AnalysisTree::Cuts("cuts", {AnalysisTree::EqualsCut("Candidates.pid", 3312)});
  // at_plainer_task->SetCuts(cuts);

  man->AddTask(at_plainer_task);

  man->Init({filename}, {"rTree"});
  man->Run(-1);// -1 = all events
  man->Finish();

  if(make_plain_ttree)
  {
    man->ClearTasks();
    std::ofstream filelist;
    filelist.open("filelist.txt");
    filelist << "intermediate_tree.root\n";
    filelist.close();

    auto* tree_task = new AnalysisTree::PlainTreeFiller();
    std::string branchname_rec = "Complex";
    tree_task->SetInputBranchNames({branchname_rec});
    tree_task->SetOutputName("analysis_plain_ttree.root", "plain_tree");
    tree_task->AddBranch(branchname_rec);

    man->AddTask(tree_task);

    man->Init({"filelist.txt"}, {"pTree"});
    man->Run(-1);// -1 = all events
    man->Finish();
  }
  return EXIT_SUCCESS;
}
