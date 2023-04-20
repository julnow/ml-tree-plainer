#include "MlTreePlainer.hpp"
#include "AnalysisTree/TaskManager.hpp"

void MlTreePlainer::Init()
{
  auto* man = AnalysisTree::TaskManager::GetInstance();
  auto* chain = man->GetChain();

  //definition of detectors and tracks
  tofhits_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Hit>*>(chain->GetPointerToBranch("TofHits"));
  simulated_= ANALYSISTREE_UTILS_GET<AnalysisTree::Particles*>(chain->GetPointerToBranch("SimParticles"));
  vtxtracks_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Track>*>(chain->GetPointerToBranch("VtxTracks"));
  trdtracks_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Track>*>(chain->GetPointerToBranch("TrdTracks"));
  richrings_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Hit>*>(chain->GetPointerToBranch("RichRings"));
  rec_event_header_ = ANALYSISTREE_UTILS_GET<AnalysisTree::EventHeader*>(chain->GetPointerToBranch("RecEventHeader"));
  //definition of matching
  tof2sim_match_ = chain->GetMatchPointers().find(config_->GetMatchName("TofHits", "SimParticles"))->second;
  vtx2tof_match_ = chain->GetMatchPointers().find(config_->GetMatchName("VtxTracks", "TofHits"))->second;
  vtx2trd_match_ = chain->GetMatchPointers().find(config_->GetMatchName("VtxTracks", "TrdTracks"))->second;
  vtx2rich_match_ = chain->GetMatchPointers().find(config_->GetMatchName("VtxTracks", "RichRings"))->second;

  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  //output tree fields (excluding defaults values like momentum etc.)
  AnalysisTree::BranchConfig out_particles("Complex", AnalysisTree::DetType::kParticle);
  //rec event
  out_particles.AddField<int>("M");
  out_particles.AddField<float>("event_vtx_chi2");
  //tof
  out_particles.AddField<float>("mass2");
  // out_particles.AddField<float>("l");
  // out_particles.AddField<float>("t");
  out_particles.AddField<float>("mass2_over_p2");
  out_particles.AddField<float>("phi_tof");
  out_particles.AddField<float>("dx_tof");
  out_particles.AddField<float>("dy_tof");
  out_particles.AddField<float>("dz_tof");
  //vtx
  out_particles.AddField<int>("q");
  out_particles.AddField<float>("chi2_vtx");
  // out_particles.AddField<int>("ndf_vtx");
  // out_particles.AddField<int>("n_hits_vtx");
  out_particles.AddField<float>("dcax");
  out_particles.AddField<float>("dcay");
  out_particles.AddField<float>("dcaz");
  //trd
  out_particles.AddField<int>("n_hits_trd");
  out_particles.AddField<float>("chi2_ov_ndf_trd");
  out_particles.AddField<float>("e_loss_0");
  out_particles.AddField<float>("e_loss_1");
  out_particles.AddField<float>("e_loss_2");
  out_particles.AddField<float>("e_loss_3");
  //rich
  out_particles.AddField<float>("axis_a_rich");
  out_particles.AddField<float>("axis_b_rich");
  out_particles.AddField<float>("radial_pos_rich");
  out_particles.AddField<float>("radial_angle_rich");
  out_particles.AddField<float>("chi2_ov_ndf_rich");
  out_particles.AddField<float>("phi_ellipse_rich");
  out_particles.AddField<float>("phi_rich");
  out_particles.AddField<float>("radius_rich");
  out_particles.AddField<int>("n_hits_rich");

  man->AddBranch(plain_branch_, out_particles);

  InitIndices();
}

void MlTreePlainer::Exec()
{
  plain_branch_ -> ClearChannels();

  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();

  //rec event variables taken once for each event handled by Exec()
  int multiplicity_r = rec_event_header_->GetField<int>(multiplicity_id_r_);
  float vtx_chi2_r = rec_event_header_->GetField<float>(vtx_chi2_id_r_);

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
              //create output row with one particle
              auto& output_particle = plain_branch_->AddChannel(out_config->GetBranchConfig(plain_branch_->GetId()));
              //RecEvent
              output_particle.SetField(multiplicity_r, multiplicity_id_w1_);
              output_particle.SetField(vtx_chi2_r, vtx_chi2_w1_);
              //Tof
              float mass2 = input_particle.GetField<float>(mass2_id_tof_);
              output_particle.SetField(mass2, mass2_id_w1_);
              float qp_tof = input_particle.GetField<float>(qp_id_tof_);
              // output_particle.SetField(input_particle.GetField<float>(l_id_tof_), l_id_w1_);
              // output_particle.SetField(input_particle.GetField<float>(t_id_tof_), t_id_w1_);
              output_particle.SetField(mass2/qp_tof/qp_tof, m2_ov_p2_id_w1_);
              output_particle.SetField(input_particle.GetField<float>(phi_id_tof_), phi_id_w1_);
              output_particle.SetField(input_particle.GetField<float>(dx_id_tof_), dx_id_w1_);
              output_particle.SetField(input_particle.GetField<float>(dy_id_tof_), dy_id_w1_);
              // output_particle.SetField(input_particle.GetField<float>(dz_id_tof_), dz_id_w1_);
              //Simulated
              auto& matched_particle_sim = simulated_->GetChannel(matched_particle_sim_id);
              output_particle.SetMass(matched_particle_sim.GetMass());
              output_particle.SetPid(matched_particle_sim.GetPid());
              //VtxTracks
              auto& matched_particle_vtx = vtxtracks_->GetChannel(matched_particle_vtx_id);
              output_particle.SetMomentum3(matched_particle_vtx.GetMomentum3());
              output_particle.SetField(matched_particle_vtx.GetField<int>(q_id_vtx_), q_id_w1_);
              output_particle.SetField(matched_particle_vtx.GetField<float>(chi2_id_vtx_), chi2_vtx_id_w1_);
              // output_particle.SetField(matched_particle_vtx.GetField<float>(ndf_id_vtx_), ndf_vtx_id_w1_);
              // output_particle.SetField(matched_particle_vtx.GetField<float>(nhits_id_vtx_), nhits_vtx_id_w1_);
              output_particle.SetField(matched_particle_vtx.GetField<float>(dcax_id_vtx_), dcax_id_w1_);
              output_particle.SetField(matched_particle_vtx.GetField<float>(dcay_id_vtx_), dcay_id_w1_);
              output_particle.SetField(matched_particle_vtx.GetField<float>(dcaz_id_vtx_), dcaz_id_w1_);
              //TrdTracks
              auto& matched_particle_trd = trdtracks_->GetChannel(matched_particle_trd_id);
              output_particle.SetField(matched_particle_trd.GetField<int>(nhits_id_trd_), nhits_trd_id_w1_);
              output_particle.SetField(matched_particle_trd.GetField<float>(chi2_ov_ndf_id_trd_), chi2_ov_ndf_trd_id_w1_);
              output_particle.SetField(matched_particle_trd.GetField<float>(energy_loss_0_id_trd_), energy_loss_0_id_w1_);
              output_particle.SetField(matched_particle_trd.GetField<float>(energy_loss_1_id_trd_), energy_loss_1_id_w1_);
              output_particle.SetField(matched_particle_trd.GetField<float>(energy_loss_2_id_trd_), energy_loss_2_id_w1_);
              output_particle.SetField(matched_particle_trd.GetField<float>(energy_loss_3_id_trd_), energy_loss_3_id_w1_);                                    
              // //RichRings
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

              }
            }
          }
        }
      }
}

void MlTreePlainer::InitIndices()
{

  //input fields
  //rec event
  auto in_branch_rec_event = config_->GetBranchConfig("RecEventHeader");
  multiplicity_id_r_       = in_branch_rec_event.GetFieldId("M");
  vtx_chi2_id_r_           = in_branch_rec_event.GetFieldId("vtx_chi2");
  //tofhits
  auto in_branch_tof   = config_->GetBranchConfig("TofHits");
  mass2_id_tof_        = in_branch_tof.GetFieldId("mass2");
  // l_id_tof_            = in_branch_tof.GetFieldId("l");
  // t_id_tof_            = in_branch_tof.GetFieldId("t");
  qp_id_tof_           = in_branch_tof.GetFieldId("qp_tof");
  phi_id_tof_          = in_branch_tof.GetFieldId("phi");
  dx_id_tof_           = in_branch_tof.GetFieldId("dx");
  dy_id_tof_           = in_branch_tof.GetFieldId("dy");
  // dz_id_tof_           = in_branch_tof.GetFieldId("dz");
  //vtx tracks
  auto in_branch_vtx   = config_->GetBranchConfig("VtxTracks");
  q_id_vtx_            = in_branch_vtx.GetFieldId("q");
  chi2_id_vtx_         = in_branch_vtx.GetFieldId("vtx_chi2");
  // ndf_id_vtx_          = in_branch_vtx.GetFieldId("ndf");
  // nhits_id_vtx_        = in_branch_vtx.GetFieldId("nhits");
  dcax_id_vtx_         = in_branch_vtx.GetFieldId("dcax");
  dcay_id_vtx_         = in_branch_vtx.GetFieldId("dcay");
  dcaz_id_vtx_         = in_branch_vtx.GetFieldId("dcaz");
  //trdtracks
  auto in_branch_trd    = config_->GetBranchConfig("TrdTracks");
  nhits_id_trd_         = in_branch_trd.GetFieldId("n_hits");
  chi2_ov_ndf_id_trd_   = in_branch_trd.GetFieldId("chi2_ov_ndf");
  energy_loss_0_id_trd_ = in_branch_trd.GetFieldId("energy_loss_0");
  energy_loss_1_id_trd_ = in_branch_trd.GetFieldId("energy_loss_1");
  energy_loss_2_id_trd_ = in_branch_trd.GetFieldId("energy_loss_2");
  energy_loss_3_id_trd_ = in_branch_trd.GetFieldId("energy_loss_3");
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

  //output tree
  auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();
  const auto& out_branch = out_config->GetBranchConfig(plain_branch_->GetId());
  // rec event
  multiplicity_id_w1_ = out_branch.GetFieldId("M");
  vtx_chi2_w1_        = out_branch.GetFieldId("event_vtx_chi2");
  // tof
  mass2_id_w1_    = out_branch.GetFieldId("mass2");
  // l_id_w1_      = out_branch.GetFieldId("l");
  // t_id_w1_      = out_branch.GetFieldId("t");
  m2_ov_p2_id_w1_ = out_branch.GetFieldId("mass2_over_p2");
  phi_id_w1_      = out_branch.GetFieldId("phi_tof");
  dx_id_w1_       = out_branch.GetFieldId("dx_tof");
  dy_id_w1_       = out_branch.GetFieldId("dy_tof");
  // dz_id_w1_     = out_branch.GetFieldId("dz_tof");
  //vtx
  q_id_w1_               = out_branch.GetFieldId("q");
  chi2_vtx_id_w1_        = out_branch.GetFieldId("chi2_vtx");
  // ndf_vtx_id_w1_         = out_branch.GetFieldId("ndf_vtx");
  // nhits_vtx_id_w1_       = out_branch.GetFieldId("n_hits_vtx");
  dcax_id_w1_            = out_branch.GetFieldId("dcax");
  dcay_id_w1_            = out_branch.GetFieldId("dcay");
  dcaz_id_w1_            = out_branch.GetFieldId("dcaz");
  //trd
  nhits_trd_id_w1_       = out_branch.GetFieldId("n_hits_trd");
  chi2_ov_ndf_trd_id_w1_ = out_branch.GetFieldId("chi2_ov_ndf_trd");
  energy_loss_0_id_w1_   = out_branch.GetFieldId("e_loss_0");
  energy_loss_1_id_w1_   = out_branch.GetFieldId("e_loss_1");
  energy_loss_2_id_w1_   = out_branch.GetFieldId("e_loss_2");
  energy_loss_3_id_w1_   = out_branch.GetFieldId("e_loss_3");
  //rich
  axis_a_rich_id_w1_     = out_branch.GetFieldId("axis_a_rich");
  axis_b_rich_id_w1_     = out_branch.GetFieldId("axis_b_rich");
  radial_pos_rich_id_w1_  = out_branch.GetFieldId("radial_pos_rich");
  radial_angle_rich_id_w1_ = out_branch.GetFieldId("radial_angle_rich");
  chi2_ov_ndf_rich_id_w1_ = out_branch.GetFieldId("chi2_ov_ndf_rich");
  phi_ellipse_rich_id_w1_ = out_branch.GetFieldId("phi_ellipse_rich");
  phi_rich_id_w1_         = out_branch.GetFieldId("phi_rich");
  radius_rich_id_w1_      = out_branch.GetFieldId("radius_rich");
  n_hits_rich_id_w1_      = out_branch.GetFieldId("n_hits_rich");

}
