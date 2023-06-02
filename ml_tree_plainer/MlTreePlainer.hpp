#ifndef MLTREEPLAINER_HPP
#define MLTREEPLAINER_HPP

#include "AnalysisTree/Cuts.hpp"
#include "AnalysisTree/Detector.hpp"
#include "AnalysisTree/EventHeader.hpp"
#include "AnalysisTree/Task.hpp"

class MlTreePlainer : public AnalysisTree::Task
{
public:
  explicit MlTreePlainer() = default;
  ~MlTreePlainer() override = default;

  void Init() override;
  void Exec() override;
  void Finish() override{};

  void InitIndices();

  // void SetCuts(AnalysisTree::Cuts* cuts) { cuts_ = cuts; }

protected:
  // input branches
  AnalysisTree::Particles *simulated_{nullptr};
  AnalysisTree::Detector<AnalysisTree::Hit> *tofhits_{nullptr};
  AnalysisTree::Detector<AnalysisTree::Track> *vtxtracks_{nullptr};
  AnalysisTree::EventHeader *rec_event_header_{nullptr};
  AnalysisTree::Matching *tof2sim_match_{nullptr};
  AnalysisTree::Matching *vtx2tof_match_{nullptr};
  AnalysisTree::Matching *vtx2sim_match_{nullptr};
  AnalysisTree::Matching* vtx2trd_match_{nullptr};
  

  // output branch
  AnalysisTree::Particles *plain_branch_{nullptr};

  //**** input fields ***********
  // event header variables
  int multiplicity_id_r_{AnalysisTree::UndefValueInt};
  // tof variables
  int mass2_id_tof_{AnalysisTree::UndefValueInt};
  int l_id_tof_{AnalysisTree::UndefValueInt};
  int t_id_tof_{AnalysisTree::UndefValueInt};

  // vtx variables
  int q_id_vtx_{AnalysisTree::UndefValueInt};
  int vtx_chi2_id_vtx_{AnalysisTree::UndefValueInt};
  int chi2_id_vtx_{AnalysisTree::UndefValueInt};
  int ndf_id_vtx_{AnalysisTree::UndefValueInt};

  // sim particles variables
  int mother_id_s_{AnalysisTree::UndefValueInt};
  int pT_id_s_{AnalysisTree::UndefValueInt};
  int p_id_s_{AnalysisTree::UndefValueInt};
  int phi_id_s_{AnalysisTree::UndefValueInt};
  int eta_id_s_{AnalysisTree::UndefValueInt};

  //trd variables
  int nhits_id_trd_{AnalysisTree::UndefValueInt};
  int chi2_ov_ndf_id_trd_{AnalysisTree::UndefValueInt};
  int energy_loss_0_id_trd_{AnalysisTree::UndefValueInt};
  int energy_loss_1_id_trd_{AnalysisTree::UndefValueInt};
  int energy_loss_2_id_trd_{AnalysisTree::UndefValueInt};
  int energy_loss_3_id_trd_{AnalysisTree::UndefValueInt};


  

  //*****************************

  //***** output fields *********
  // from event
  int multiplicity_id_w1_{AnalysisTree::UndefValueInt};
  // from tof
  int mass2_id_w1_{AnalysisTree::UndefValueInt};
  int v_tof_id_w1_{AnalysisTree::UndefValueInt};
  int l_id_w1_{AnalysisTree::UndefValueInt};
  int t_id_w1_{AnalysisTree::UndefValueInt};
  // from vtx
  int q_id_w1_{AnalysisTree::UndefValueInt};
  int vtx_chi2_id_w1_{AnalysisTree::UndefValueInt};
  int chi2_ov_ndf_id_w1_{AnalysisTree::UndefValueInt};
  // from sim
  int mother_id_w1_{AnalysisTree::UndefValueInt};
  int sim_pT_w1_{AnalysisTree::UndefValueInt};
  int sim_p_w1_{AnalysisTree::UndefValueInt};
  int sim_phi_w1_{AnalysisTree::UndefValueInt};
  int sim_eta_w1_{AnalysisTree::UndefValueInt};
  //from trd
  int nhits_trd_id_w1_{AnalysisTree::UndefValueInt};
  int chi2_ov_ndf_trd_id_w1_{AnalysisTree::UndefValueInt};
  int energy_loss_0_id_w1_{AnalysisTree::UndefValueInt};
  int energy_loss_1_id_w1_{AnalysisTree::UndefValueInt};
  int energy_loss_2_id_w1_{AnalysisTree::UndefValueInt};
  int energy_loss_3_id_w1_{AnalysisTree::UndefValueInt};

  //******************************
};
#endif // MLTREEPLAINER_HPP
