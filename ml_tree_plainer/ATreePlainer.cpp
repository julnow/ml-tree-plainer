#include "ATreePlainer.hpp"

#include "AnalysisTree/TaskManager.hpp"

void ATreePlainer::Init()
{
  auto* man = AnalysisTree::TaskManager::GetInstance();
  auto* chain = man->GetChain();

  //definition of detectors and tracks
  tofhits_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Hit>*>(chain->GetPointerToBranch("TofHits"));
  simulated_= ANALYSISTREE_UTILS_GET<AnalysisTree::Particles*>(chain->GetPointerToBranch("SimParticles"));
  vtxtracks_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Track>*>(chain->GetPointerToBranch("VtxTracks"));
  trdtracks_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Track>*>(chain->GetPointerToBranch("TrdTracks"));
  richrings_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Hit>*>(chain->GetPointerToBranch("RichRings"));
  //definition of matching
  tof2sim_match_ = chain->GetMatchPointers().find(config_->GetMatchName("TofHits", "SimParticles"))->second;
  vtx2tof_match_ = chain->GetMatchPointers().find(config_->GetMatchName("VtxTracks", "TofHits"))->second;
  vtx2trd_match_ = chain->GetMatchPointers().find(config_->GetMatchName("VtxTracks", "TrdTracks"))->second;
  vtx2rich_match_ = chain->GetMatchPointers().find(config_->GetMatchName("VtxTracks", "RichRings"))->second;


  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  //output tree fields (excluding defaults values like momentum etc.)
  AnalysisTree::BranchConfig out_particles("Complex", AnalysisTree::DetType::kParticle);
  out_particles.AddField<float>("mass2");
  out_particles.AddField<int>("q");
  out_particles.AddField<float>("chi2_vtx");
  out_particles.AddField<float>("trd_proba_e");
  out_particles.AddField<float>("trd_proba_pi");
  out_particles.AddField<float>("trd_proba_k");
  out_particles.AddField<float>("trd_proba_p");
  out_particles.AddField<int>("nhits_trd");
  out_particles.AddField<float>("axis_a_rich");
  out_particles.AddField<float>("axis_b_rich");
  out_particles.AddField<float>("radial_pos_rich");
  out_particles.AddField<float>("radial_angle_rich");
  out_particles.AddField<float>("chi2_ov_ndf_rich");
  out_particles.AddField<float>("phi_ellipse_rich");
  out_particles.AddField<float>("phi_rich");
  out_particles.AddField<float>("radius_rich");
  out_particles.AddField<int>("n_hits_rich");
  // out_particles.AddField<int>("n_hits_on_ring_rich");

  man->AddBranch(plain_branch_, out_particles);

  InitIndices();
}

void ATreePlainer::Exec()
{
  plain_branch_ -> ClearChannels();

  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();

   for(auto& input_particle : *tofhits_)
   {

      //gets particles which left track in each detector
       const auto matched_particle_sim_id = tof2sim_match_->GetMatch(input_particle.GetId());
       if (matched_particle_sim_id > 0){
         const auto matched_particle_vtx_id = vtx2tof_match_->GetMatchInverted(input_particle.GetId());
         if (matched_particle_vtx_id > 0){
            const auto matched_particle_trd_id = vtx2trd_match_->GetMatch(matched_particle_vtx_id);//input_particle.GetId());
          if (matched_particle_trd_id > 0){
            const auto matched_particle_rich_id = vtx2rich_match_->GetMatch(matched_particle_vtx_id);
            if (matched_particle_rich_id>0){

            //from Tof
              auto& output_particle = plain_branch_->AddChannel(out_config->GetBranchConfig(plain_branch_->GetId()));
              output_particle.SetField(input_particle.GetField<float>(mass2_id_tof_), mass2_id_w1_);
              //from Simulated
              auto& matched_particle_sim = simulated_->GetChannel(matched_particle_sim_id);
              output_particle.SetMass(matched_particle_sim.GetMass());
              output_particle.SetPid(matched_particle_sim.GetPid());
              //from VtxTracks
              auto& matched_particle_vtx = vtxtracks_->GetChannel(matched_particle_vtx_id);
              output_particle.SetMomentum3(matched_particle_vtx.GetMomentum3());
              output_particle.SetField(matched_particle_vtx.GetField<int>(q_id_vtx_), q_id_w1_);
              output_particle.SetField(matched_particle_vtx.GetField<float>(chi2_id_vtx_), chi2_vtx_id_w1_);
              //from TrdTracks
              auto& matched_particle_trd = trdtracks_->GetChannel(matched_particle_trd_id);
              output_particle.SetField(matched_particle_trd.GetField<float>(pe_id_trd_), pe_id_w1_);
              output_particle.SetField(matched_particle_trd.GetField<float>(ppi_id_trd_), ppi_id_w1_);
              output_particle.SetField(matched_particle_trd.GetField<float>(pk_id_trd_), pk_id_w1_);
              output_particle.SetField(matched_particle_trd.GetField<float>(pp_id_trd_), pp_id_w1_);
              output_particle.SetField(matched_particle_trd.GetField<int>(nhits_id_trd_), nhits_trd_id_w1_);
              //from RichRings
              auto& matched_particle_rich = richrings_->GetChannel(matched_particle_rich_id);
              output_particle.SetField(matched_particle_rich.GetField<float>(axis_a_id_rich_), axis_a_rich_id_w1_);
              output_particle.SetField(matched_particle_rich.GetField<float>(axis_b_id_rich_), axis_b_rich_id_w1_);
              output_particle.SetField(matched_particle_rich.GetField<float>(radial_pos_id_rich_), radial_pos_rich_id_w1_);
              output_particle.SetField(matched_particle_rich.GetField<float>(radial_angle_id_rich_), radial_angle_rich_id_w1_);
              output_particle.SetField(matched_particle_rich.GetField<float>(chi2_ov_ndf_id_rich_), chi2_ov_ndf_rich_id_w1_);
              output_particle.SetField(matched_particle_rich.GetField<float>(phi_ellipse_id_rich_), phi_ellipse_rich_id_w1_);
              output_particle.SetField(matched_particle_rich.GetField<float>(phi_id_rich_), phi_rich_id_w1_);              
              output_particle.SetField(matched_particle_rich.GetField<float>(radius_id_rich_), radius_rich_id_w1_);
              output_particle.SetField(matched_particle_rich.GetField<int>(n_hits_id_rich_), n_hits_rich_id_w1_);
              // output_particle.SetField(matched_particle_rich.GetField<int>(n_hits_on_ring_id_rich_), n_hits_on_ring_rich_id_w1_);

              }
            }
          }
        }
      }
}

void ATreePlainer::InitIndices()
{
  //input fields
  //tofhits
   auto in_branch_tof   = config_->GetBranchConfig("TofHits");
   mass2_id_tof_        = in_branch_tof.GetFieldId("mass2");

  //vtx tracks
   auto in_branch_vtx   = config_->GetBranchConfig("VtxTracks");
   q_id_vtx_            = in_branch_vtx.GetFieldId("q");
   chi2_id_vtx_         = in_branch_vtx.GetFieldId("vtx_chi2");

  //trdtracks
   auto in_branch_trd   = config_->GetBranchConfig("TrdTracks");
   pe_id_trd_           = in_branch_trd.GetFieldId("pid_like_e");
   ppi_id_trd_          = in_branch_trd.GetFieldId("pid_like_pi");
   pk_id_trd_           = in_branch_trd.GetFieldId("pid_like_k");
   pp_id_trd_           = in_branch_trd.GetFieldId("pid_like_p");
   nhits_id_trd_        = in_branch_trd.GetFieldId("n_hits");

   //rich ring
  auto in_branch_rich   = config_->GetBranchConfig("RichRings");
  axis_a_id_rich_       = in_branch_rich.GetFieldId("axis_a");
  axis_b_id_rich_       = in_branch_rich.GetFieldId("axis_b");
  radial_pos_id_rich_   = in_branch_rich.GetFieldId("radial_pos");
  radial_angle_id_rich_ = in_branch_rich.GetFieldId("radial_angle");
  chi2_ov_ndf_id_rich_  = in_branch_rich.GetFieldId("chi2_ov_ndf");
  phi_ellipse_id_rich_  = in_branch_rich.GetFieldId("phi_ellipse");
  phi_id_rich_          = in_branch_rich.GetFieldId("phi");
  radius_id_rich_       = in_branch_rich.GetFieldId("radius");
  n_hits_id_rich_       = in_branch_rich.GetFieldId("n_hits");
  // n_hits_on_ring_id_rich_ = in_branch_rich.GetFieldId("n_hits_on_ring");


  //output tree
  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto& out_branch = out_config->GetBranchConfig(plain_branch_->GetId());
  mass2_id_w1_           = out_branch.GetFieldId("mass2");
  q_id_w1_               = out_branch.GetFieldId("q");
  chi2_vtx_id_w1_        = out_branch.GetFieldId("chi2_vtx");
  pe_id_w1_              = out_branch.GetFieldId("trd_proba_e");
  ppi_id_w1_             = out_branch.GetFieldId("trd_proba_pi");
  pk_id_w1_              = out_branch.GetFieldId("trd_proba_k");
  pp_id_w1_              = out_branch.GetFieldId("trd_proba_p");
  nhits_trd_id_w1_       = out_branch.GetFieldId("nhits_trd");
  axis_a_rich_id_w1_     = out_branch.GetFieldId("axis_a_rich");
  axis_b_rich_id_w1_     = out_branch.GetFieldId("axis_b_rich");
  radial_pos_rich_id_w1_  = out_branch.GetFieldId("radial_pos_rich");
  radial_angle_rich_id_w1_ = out_branch.GetFieldId("radial_angle_rich");
  chi2_ov_ndf_rich_id_w1_ = out_branch.GetFieldId("chi2_ov_ndf_rich");
  phi_ellipse_rich_id_w1_ = out_branch.GetFieldId("phi_ellipse_rich");
  phi_rich_id_w1_         = out_branch.GetFieldId("phi_rich");
  radius_rich_id_w1_      = out_branch.GetFieldId("radius_rich");
  n_hits_rich_id_w1_      = out_branch.GetFieldId("n_hits_rich");
  // n_hits_on_ring_rich_id_w1_ = out_branch.GetFieldId("n_hits_on_ring_rich");

}
