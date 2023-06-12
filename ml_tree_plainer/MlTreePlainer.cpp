#include "MlTreePlainer.hpp"
#include "AnalysisTree/TaskManager.hpp"

void MlTreePlainer::Init()
{
  auto *man = AnalysisTree::TaskManager::GetInstance();
  auto *chain = man->GetChain();

  // definition of detectors and tracks
  tofhits_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Hit> *>(chain->GetPointerToBranch("TofHits"));
  simulated_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Particles *>(chain->GetPointerToBranch("SimParticles"));
  rec_particles_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Particles *>(chain->GetPointerToBranch("RecParticles"));
  rec_event_header_ = ANALYSISTREE_UTILS_GET<AnalysisTree::EventHeader *>(chain->GetPointerToBranch("RecEventHeader"));
  // definition of matching
  tof2sim_match_ = chain->GetMatchPointers().find(config_->GetMatchName("TofHits", "SimParticles"))->second;
  rec2sim_match_ = chain->GetMatchPointers().find(config_->GetMatchName("RecParticles", "TofHits"))->second;
  rec2tof_match_ = chain->GetMatchPointers().find(config_->GetMatchName("RecParticles", "TofHits"))->second;



  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  // output tree fields (excluding defaults values like momentum etc.)
  AnalysisTree::BranchConfig out_particles("Complex", AnalysisTree::DetType::kParticle);
  // rec event
  out_particles.AddField<int>("M");
  // tof
  out_particles.AddField<float>("mass2");
  out_particles.AddField<float>("l");
  out_particles.AddField<float>("t");
  out_particles.AddField<float>("v_tof");
  // sim
  out_particles.AddField<int>("mother_id");
  out_particles.AddField<float>("sim_pT");
  out_particles.AddField<float>("sim_p");
  out_particles.AddField<float>("sim_phi");
  out_particles.AddField<float>("sim_eta");
  // vtx
  out_particles.AddField<int>("q");
  out_particles.AddField<float>("vtx_chi2");
  out_particles.AddField<float>("chi2_ov_ndf_vtx");
  //rec
  out_particles.AddField<int>("gauss_pid");
  out_particles.AddField<float>("prob_K");
  out_particles.AddField<float>("prob_p");
  out_particles.AddField<float>("prob_pi");

  man->AddBranch(plain_branch_, out_particles);

  InitIndices();
}

void MlTreePlainer::Exec()
{
  plain_branch_->ClearChannels();

  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();

  // rec event variables taken once for each event handled by Exec()
  int multiplicity_r = rec_event_header_->GetField<int>(multiplicity_id_r_);

  for (auto &input_particle : *tofhits_)
  {
    // gets particles which left tracks in tof
    const auto matched_particle_sim_id = tof2sim_match_->GetMatch(input_particle.GetId());
    if (matched_particle_sim_id > 0)
    {
      // Simulated
      auto &matched_particle_sim = simulated_->GetChannel(matched_particle_sim_id);
      int mother_id = matched_particle_sim.GetField<int>(mother_id_s_);
      // only matched vtx to tof and same sim
      const auto matched_particle_vtx_id = rec2tof_match_->GetMatchInverted(input_particle.GetId());
      auto &matched_particle_vtx = rec_particles_->GetChannel(matched_particle_vtx_id);
      if (matched_particle_vtx_id > 0)
      {
        // const auto matched_particle_sim_vtx_id = rec2sim_match_->GetMatch(matched_particle_vtx_id);
        // if (matched_particle_sim_vtx_id == matched_particle_sim_id)
        // {
          
          auto &output_particle = plain_branch_->AddChannel(out_config->GetBranchConfig(plain_branch_->GetId()));
          output_particle.SetMass(matched_particle_sim.GetMass());
          output_particle.SetPid(matched_particle_sim.GetPid());
          output_particle.SetField(mother_id, mother_id_w1_);
          output_particle.SetField(matched_particle_sim.GetField<float>(pT_id_s_), sim_pT_w1_);
          output_particle.SetField(matched_particle_sim.GetField<float>(p_id_s_), sim_p_w1_);
          output_particle.SetField(matched_particle_sim.GetField<float>(phi_id_s_), sim_phi_w1_);
          output_particle.SetField(matched_particle_sim.GetField<float>(eta_id_s_), sim_eta_w1_);

          // RecEvent
          output_particle.SetField(multiplicity_r, multiplicity_id_w1_);
          // Tof
          output_particle.SetField(input_particle.GetField<float>(mass2_id_tof_), mass2_id_w1_);
          float l_tof = input_particle.GetField<float>(l_id_tof_);
          output_particle.SetField(l_tof, l_id_w1_);
          float t_tof = input_particle.GetField<float>(t_id_tof_);
          output_particle.SetField(t_tof, t_id_w1_);
          output_particle.SetField(l_tof / t_tof, v_tof_id_w1_);

          // // VtxTracks
          // output_particle.SetMomentum3(matched_particle_vtx.GetMomentum3());
          // output_particle.SetField(matched_particle_vtx.GetField<int>(q_id_vtx_), q_id_w1_);
          // output_particle.SetField(matched_particle_vtx.GetField<float>(vtx_chi2_id_vtx_), vtx_chi2_id_w1_);
          // float chi2_vtx = matched_particle_vtx.GetField<float>(chi2_id_vtx_);
          // int ndf_vtx = matched_particle_vtx.GetField<int>(ndf_id_vtx_);
          // output_particle.SetField(chi2_vtx / ndf_vtx, chi2_ov_ndf_id_w1_);
          // // rec
          // output_particle.SetField(matched_particle_vtx.GetField<int>(gauss_pid_id_vtx_), gauss_pid_id_w1_);
          // output_particle.SetField(matched_particle_vtx.GetField<float>(prob_K_id_vtx_), prob_K_id_w1_);
          // output_particle.SetField(matched_particle_vtx.GetField<float>(prob_p_id_vtx_), prob_p_id_w1_);
          // output_particle.SetField(matched_particle_vtx.GetField<float>(prob_pi_id_vtx_), prob_pi_id_w1_);

        // }
      }
    }
  }
}

void MlTreePlainer::InitIndices()
{

  // input fields

  // rec event
  auto in_branch_rec_event = config_->GetBranchConfig("RecEventHeader");
  multiplicity_id_r_ = in_branch_rec_event.GetFieldId("M");

  // tofhits
  auto in_branch_tof = config_->GetBranchConfig("TofHits");
  mass2_id_tof_ = in_branch_tof.GetFieldId("mass2");
  l_id_tof_ = in_branch_tof.GetFieldId("l");
  t_id_tof_ = in_branch_tof.GetFieldId("t");

  // simevents
  auto in_branch_sim = config_->GetBranchConfig("SimParticles");
  mother_id_s_ = in_branch_sim.GetFieldId("mother_id");
  pT_id_s_ = in_branch_sim.GetFieldId("pT");
  p_id_s_ = in_branch_sim.GetFieldId("p");
  phi_id_s_ = in_branch_sim.GetFieldId("phi");
  eta_id_s_ = in_branch_sim.GetFieldId("eta");

  // vtx tracks
  auto in_branch_vtx = config_->GetBranchConfig("RecParticles");
  q_id_vtx_ = in_branch_vtx.GetFieldId("q");
  vtx_chi2_id_vtx_ = in_branch_vtx.GetFieldId("vtx_chi2");
  chi2_id_vtx_ = in_branch_vtx.GetFieldId("chi2");
  ndf_id_vtx_ = in_branch_vtx.GetFieldId("ndf");
  // rec
  gauss_pid_id_vtx_ = in_branch_vtx.GetFieldId("pid");
  prob_K_id_vtx_ = in_branch_vtx.GetFieldId("prob_K");
  prob_p_id_vtx_ = in_branch_vtx.GetFieldId("prob_p");
  prob_pi_id_vtx_ = in_branch_vtx.GetFieldId("prob_pi");
  

  // output tree
  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto &out_branch = out_config->GetBranchConfig(plain_branch_->GetId());
  // rec event
  multiplicity_id_w1_ = out_branch.GetFieldId("M");
  // tof
  mass2_id_w1_ = out_branch.GetFieldId("mass2");
  l_id_w1_ = out_branch.GetFieldId("l");
  t_id_w1_ = out_branch.GetFieldId("t");
  v_tof_id_w1_ = out_branch.GetFieldId("v_tof");
  // vtx
  q_id_w1_ = out_branch.GetFieldId("q");
  vtx_chi2_id_w1_ = out_branch.GetFieldId("vtx_chi2");
  chi2_ov_ndf_id_w1_ = out_branch.GetFieldId("chi2_ov_ndf_vtx");
  // sim
  mother_id_w1_ = out_branch.GetFieldId("mother_id");
  sim_pT_w1_ = out_branch.GetFieldId("sim_pT");
  sim_p_w1_ = out_branch.GetFieldId("sim_p");
  sim_phi_w1_ = out_branch.GetFieldId("sim_phi");
  sim_eta_w1_ = out_branch.GetFieldId("sim_eta");
  //rec
  gauss_pid_id_w1_ = out_branch.GetFieldId("gauss_pid");
  prob_K_id_w1_ = out_branch.GetFieldId("prob_K");
  prob_p_id_w1_ = out_branch.GetFieldId("prob_p");
  prob_pi_id_w1_ = out_branch.GetFieldId("prob_pi");

}
