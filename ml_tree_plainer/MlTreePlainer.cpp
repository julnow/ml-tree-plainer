#include "MlTreePlainer.hpp"
#include "AnalysisTree/TaskManager.hpp"

void MlTreePlainer::Init()
{
  auto *man = AnalysisTree::TaskManager::GetInstance();
  auto *chain = man->GetChain();

  // definition of detectors and tracks
  tofhits_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Hit> *>(chain->GetPointerToBranch("TofHits"));
  simulated_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Particles *>(chain->GetPointerToBranch("SimParticles"));
  vtxtracks_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Track> *>(chain->GetPointerToBranch("VtxTracks"));
  rec_event_header_ = ANALYSISTREE_UTILS_GET<AnalysisTree::EventHeader *>(chain->GetPointerToBranch("RecEventHeader"));
  // definition of matching
  tof2sim_match_ = chain->GetMatchPointers().find(config_->GetMatchName("TofHits", "SimParticles"))->second;
  vtx2tof_match_ = chain->GetMatchPointers().find(config_->GetMatchName("VtxTracks", "TofHits"))->second;

  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  // output tree fields (excluding defaults values like momentum etc.)
  AnalysisTree::BranchConfig out_particles("Complex", AnalysisTree::DetType::kParticle);
  // rec event
  out_particles.AddField<int>("M");
  // tof
  out_particles.AddField<float>("mass2");
  out_particles.AddField<float>("l");
  out_particles.AddField<float>("t");
  out_particles.AddField<float>("mass2_over_p2");
  // sim
  out_particles.AddField<int>("mother_id");
  out_particles.AddField<float>("sim_pT");
  out_particles.AddField<float>("sim_p");
  out_particles.AddField<float>("sim_phi");
  // vtx
  out_particles.AddField<int>("q");
  out_particles.AddField<float>("vtx_chi2");
  out_particles.AddField<float>("chi2_ov_ndf_vtx");

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
    if (matched_particle_sim_id > 0){
        // Simulated
        auto &matched_particle_sim = simulated_->GetChannel(matched_particle_sim_id);
        int mother_id = matched_particle_sim.GetField<int>(mother_id_s_);
        //only primaries
        if (mother_id<0){
        auto &output_particle = plain_branch_->AddChannel(out_config->GetBranchConfig(plain_branch_->GetId()));
        output_particle.SetMass(matched_particle_sim.GetMass());
        output_particle.SetPid(matched_particle_sim.GetPid());
        output_particle.SetField(mother_id, mother_id_w1_);
        output_particle.SetField(matched_particle_sim.GetField<float>(pT_id_s_), sim_pT_w1_);
        output_particle.SetField(matched_particle_sim.GetField<float>(p_id_s_), sim_p_w1_);
        output_particle.SetField(matched_particle_sim.GetField<float>(phi_id_s_), sim_phi_w1_);

        // RecEvent
        output_particle.SetField(multiplicity_r, multiplicity_id_w1_);
        // Tof
        float mass2 = input_particle.GetField<float>(mass2_id_tof_);
        output_particle.SetField(mass2, mass2_id_w1_);
        float qp_tof = input_particle.GetField<float>(qp_id_tof_);
        output_particle.SetField(input_particle.GetField<float>(l_id_tof_), l_id_w1_);
        output_particle.SetField(input_particle.GetField<float>(t_id_tof_), t_id_w1_);
        output_particle.SetField(mass2 / qp_tof / qp_tof, m2_ov_p2_id_w1_);
        
        const auto matched_particle_vtx_id = vtx2tof_match_->GetMatchInverted(input_particle.GetId());
        if (matched_particle_vtx_id > 0)
        {
          // VtxTracks
          auto &matched_particle_vtx = vtxtracks_->GetChannel(matched_particle_vtx_id);
          output_particle.SetMomentum3(matched_particle_vtx.GetMomentum3());
          output_particle.SetField(matched_particle_vtx.GetField<int>(q_id_vtx_), q_id_w1_);
          output_particle.SetField(matched_particle_vtx.GetField<float>(vtx_chi2_id_vtx_), vtx_chi2_id_w1_);
          float chi2_vtx = matched_particle_vtx.GetField<float>(chi2_id_vtx_);
          int ndf_vtx = matched_particle_vtx.GetField<int>(ndf_id_vtx_);
          output_particle.SetField(chi2_vtx / ndf_vtx, chi2_ov_ndf_id_w1_);
        }
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
  qp_id_tof_ = in_branch_tof.GetFieldId("qp_tof");

  // simevents
  auto in_branch_sim = config_->GetBranchConfig("SimParticles");
  mother_id_s_ = in_branch_sim.GetFieldId("mother_id");
  pT_id_s_ = in_branch_sim.GetFieldId("pT");
  p_id_s_ = in_branch_sim.GetFieldId("p");
  phi_id_s_ = in_branch_sim.GetFieldId("phi");

  // vtx tracks
  auto in_branch_vtx = config_->GetBranchConfig("VtxTracks");
  q_id_vtx_ = in_branch_vtx.GetFieldId("q");
  vtx_chi2_id_vtx_ = in_branch_vtx.GetFieldId("vtx_chi2");
  chi2_id_vtx_ = in_branch_vtx.GetFieldId("chi2");
  ndf_id_vtx_ = in_branch_vtx.GetFieldId("ndf");

  // output tree
  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto &out_branch = out_config->GetBranchConfig(plain_branch_->GetId());
  // rec event
  multiplicity_id_w1_ = out_branch.GetFieldId("M");
  // tof
  mass2_id_w1_ = out_branch.GetFieldId("mass2");
  l_id_w1_ = out_branch.GetFieldId("l");
  t_id_w1_ = out_branch.GetFieldId("t");
  m2_ov_p2_id_w1_ = out_branch.GetFieldId("mass2_over_p2");
  // vtx
  q_id_w1_ = out_branch.GetFieldId("q");
  vtx_chi2_id_w1_ = out_branch.GetFieldId("vtx_chi2");
  chi2_ov_ndf_id_w1_ = out_branch.GetFieldId("chi2_ov_ndf_vtx");
  // sim
  mother_id_w1_ = out_branch.GetFieldId("mother_id");
  sim_pT_w1_ = out_branch.GetFieldId("sim_pT");
  sim_p_w1_ = out_branch.GetFieldId("sim_p");
  sim_phi_w1_ = out_branch.GetFieldId("sim_phi");
}
