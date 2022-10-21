#include "ATreePlainer.hpp"

#include "AnalysisTree/TaskManager.hpp"

void ATreePlainer::Init()
{
  auto* man = AnalysisTree::TaskManager::GetInstance();
  auto* chain = man->GetChain();

  tofhits_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Hit>*>(chain->GetPointerToBranch("TofHits"));
  simulated_= ANALYSISTREE_UTILS_GET<AnalysisTree::Particles*>(chain->GetPointerToBranch("SimParticles"));
  vtxtracks_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Track>*>(chain->GetPointerToBranch("VtxTracks"));
  trdtracks_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Track>*>(chain->GetPointerToBranch("TrdTracks"));

  tof2sim_match_ = chain->GetMatchPointers().find(config_->GetMatchName("TofHits", "SimParticles"))->second;
  vtx2tof_match_ = chain->GetMatchPointers().find(config_->GetMatchName("VtxTracks", "TofHits"))->second;
  vtx2trd_match_ = chain->GetMatchPointers().find(config_->GetMatchName("VtxTracks", "TrdTracks"))->second;

  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  //
  AnalysisTree::BranchConfig out_particles("Complex", AnalysisTree::DetType::kParticle);
  // out_particles.AddField<float>("l");
  // out_particles.AddField<float>("t");
  out_particles.AddField<float>("mass2");
  out_particles.AddField<int>("q");


  man->AddBranch(plain_branch_, out_particles);

//   if(cuts_)
//     cuts_ -> Init(*out_config);
//
  InitIndices();
}

void ATreePlainer::Exec()
{
  plain_branch_ -> ClearChannels();

  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();

   for(auto& input_particle : *tofhits_)
   {

       const auto matched_particle_sim_id = tof2sim_match_->GetMatch(input_particle.GetId());
       if (matched_particle_sim_id > 0){
         const auto matched_particle_vtx_id = vtx2tof_match_->GetMatchInverted(input_particle.GetId());
         if (matched_particle_vtx_id > 0){
            const auto matched_particle_trd_id = vtx2trd_match_->GetMatch(input_particle.GetId());
          if (matched_particle_trd_id > 0){
           //from tof
            auto& output_particle = plain_branch_->AddChannel(out_config->GetBranchConfig(plain_branch_->GetId()));
            // output_particle.SetField(input_particle.GetField<float>(l_id_tof_), l_id_w1_);
            // output_particle.SetField(input_particle.GetField<float>(t_id_tof_), t_id_w1_);
            output_particle.SetField(input_particle.GetField<float>(mass2_id_tof_), mass2_id_w1_);
            //from simulated
            auto& matched_particle_sim = simulated_->GetChannel(matched_particle_sim_id);
            output_particle.SetMass(matched_particle_sim.GetMass());
            output_particle.SetPid(matched_particle_sim.GetPid());
            //from VtxTracks
            auto& matched_particle_vtx = vtxtracks_->GetChannel(matched_particle_vtx_id);
            output_particle.SetMomentum3(matched_particle_vtx.GetMomentum3());
            output_particle.SetField(matched_particle_vtx.GetField<int>(q_id_vtx_), q_id_w1_);
            output_particle.SetField(matched_particle_vtx.GetField<int>(chi2_id_vtx_), chi2_vtx_id_w1_);
            //from TrdTracks
            auto& matched_particle_trd = trdtracks_->GetChannel(matched_particle_trd_id);
            output_particle.SetField(matched_particle_vtx.GetField<int>(pe_id_trd_), pe_id_w1_);
            output_particle.SetField(matched_particle_vtx.GetField<int>(ppi_id_trd_), pe_id_w1_);
            output_particle.SetField(matched_particle_vtx.GetField<int>(pk_id_trd_), pe_id_w1_);
            output_particle.SetField(matched_particle_vtx.GetField<int>(pp_id_trd_), pe_id_w1_);
            output_particle.SetField(matched_particle_vtx.GetField<int>(nhits_id_trd_), nhits_trd_id_w1_);


            }
          }
        }
      }
}

void ATreePlainer::InitIndices()
{
  //tofhits
   auto in_branch_tof = config_->GetBranchConfig("TofHits");
   mass2_id_tof_     = in_branch_tof.GetFieldId("mass2");

  //vtx tracks
   auto in_branch_vtx = config_->GetBranchConfig("VtxTracks");
   q_id_vtx_         = in_branch_vtx.GetFieldId("q");
   chi2_id_vtx_         = in_branch_vtx.GetFieldId("vtx_chi2");

  //trdtracks
   auto in_branch_trd = config_->GetBranchConfig("TrdTracks");
   pe_id_trd_ = in_branch_trd.GetFieldId("pid_like_e");
   ppi_id_trd_ = in_branch_trd.GetFieldId("pid_like_pi");
   pk_id_trd_ = in_branch_trd.GetFieldId("pid_like_k");
   pp_id_trd_ = in_branch_trd.GetFieldId("pid_like_p");
   nhits_id_trd_ = in_branch_trd.GetFieldId("n_hits");


   auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();
   const auto& out_branch = out_config->GetBranchConfig(plain_branch_->GetId());
   // l_id_w1_               = out_branch.GetFieldId("l");
   // t_id_w1_               = out_branch.GetFieldId("t");
   mass2_id_w1_           = out_branch.GetFieldId("mass2");
   q_id_w1_               = out_branch.GetFieldId("q");
   chi2_vtx_id_w1_       = out_branch.GetFieldId("chi2_vtx");
   pe_id_w1_              = out_branch.GetFieldId("trd_proba_e");
   ppi_id_w1_             = out_branch.GetFieldId("trd_proba_pi");
   pk_id_w1_              = out_branch.GetFieldId("trd_proba_k");
   pp_id_w1_              = out_branch.GetFieldId("trd_proba_p");
   nhits_trd_id_w1_       = out_branch.GetFieldId("nhits_trd");


}
