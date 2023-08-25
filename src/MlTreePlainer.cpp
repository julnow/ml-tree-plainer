#include "MlTreePlainer.hpp"
#include "AnalysisTree/TaskManager.hpp"

void MlTreePlainer::Init()
{
    auto* man = AnalysisTree::TaskManager::GetInstance();
    auto* chain = man->GetChain();

    // definition of detectors and tracks
    tofhits_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Hit>*>(
        chain->GetPointerToBranch("TofHits"));
    simulated_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Particles*>(
        chain->GetPointerToBranch("SimParticles"));
    vtxtracks_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Track>*>(
        chain->GetPointerToBranch("VtxTracks"));
    rec_event_header_ = ANALYSISTREE_UTILS_GET<AnalysisTree::EventHeader*>(
        chain->GetPointerToBranch("RecEventHeader"));
    trdtracks_ = ANALYSISTREE_UTILS_GET<AnalysisTree::Detector<AnalysisTree::Track>*>(
        chain->GetPointerToBranch("TrdTracks"));
    // definition of matching
    tof2sim_match_ = chain->GetMatchPointers()
                         .find(config_->GetMatchName("TofHits", "SimParticles"))
                         ->second;
    vtx2tof_match_ = chain->GetMatchPointers()
                         .find(config_->GetMatchName("VtxTracks", "TofHits"))
                         ->second;
    vtx2sim_match_ =
        chain->GetMatchPointers()
            .find(config_->GetMatchName("VtxTracks", "SimParticles"))
            ->second;
    vtx2trd_match_ = chain->GetMatchPointers()
                         .find(config_->GetMatchName("VtxTracks", "TrdTracks"))
                         ->second;

    auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();
    // output tree fields (excluding defaults values like momentum etc.)
    AnalysisTree::BranchConfig out_particles(
        json_config_.output_branch_name, AnalysisTree::DetType::kParticle);

    auto event_header_config = FindBranch("RecEventHeader");
    auto tof_config = FindBranch("TofHits");
    auto vtx_config = FindBranch("VtxTracks");
    auto sim_config = FindBranch("SimParticles");
    auto trd_config = FindBranch("TrdTracks");

    event_header_ = new MLTP::Branch(event_header_config, config_, out_particles);
    tof_ = new MLTP::Branch(tof_config, config_, out_particles);
    vtx_ = new MLTP::Branch(vtx_config, config_, out_particles);
    sim_ = new MLTP::Branch(sim_config, config_, out_particles);
    trd_ = new MLTP::Branch(trd_config, config_, out_particles);

    event_header_->AddFields();
    tof_->AddFields();
    vtx_->AddFields();
    sim_->AddFields();
    trd_->AddFields();

    man->AddBranch(plain_branch_, out_particles);

    InitIndices();
}

void MlTreePlainer::Exec()
{
    plain_branch_->ClearChannels();

    auto out_config = AnalysisTree::TaskManager::GetInstance()->GetConfig();

    for(auto& input_particle: *tofhits_)
    {
        const auto matched_particle_sim_id =
            tof2sim_match_->GetMatch(input_particle.GetId());
        if(matched_particle_sim_id > 0)
        {
            auto& matched_particle_sim = simulated_->GetChannel(matched_particle_sim_id);

            const auto matched_particle_vtx_id =
                vtx2tof_match_->GetMatchInverted(input_particle.GetId());
            auto& matched_particle_vtx = vtxtracks_->GetChannel(matched_particle_vtx_id);

            if(matched_particle_vtx_id > 0)
            {
                const auto matched_particle_sim_vtx_id =
                    vtx2sim_match_->GetMatch(matched_particle_vtx_id);
                if(matched_particle_sim_vtx_id == matched_particle_sim_id)
                {
                    namespace at = AnalysisTree;
                    auto& output_particle = plain_branch_->AddChannel(
                        out_config->GetBranchConfig(plain_branch_->GetId()));
                    vtx_->SetFields<at::Track>(output_particle, matched_particle_vtx);
                    tof_->SetFields<at::Hit>(output_particle, input_particle);
                    sim_->SetFields<at::Particle>(output_particle, matched_particle_sim);
                    event_header_->SetFields<at::EventHeader>(output_particle, *rec_event_header_);

                    const auto matched_particle_trd_id =
                        vtx2trd_match_->GetMatch(matched_particle_vtx_id);
                    if(matched_particle_trd_id > 0)
                    {
                        // TrdTracks
                        auto& matched_particle_trd =
                            trdtracks_->GetChannel(matched_particle_trd_id);
                        trd_->SetFields(output_particle, matched_particle_trd);
                    }
                }
            }
        }
    }
}

void MlTreePlainer::InitIndices()
{
    event_header_->InitInIndices();
    tof_->InitInIndices();
    vtx_->InitInIndices();
    sim_->InitInIndices();
    trd_->InitInIndices();

    event_header_->InitOutIndices();
    tof_->InitOutIndices();
    vtx_->InitOutIndices();
    sim_->InitOutIndices();
    trd_->InitOutIndices();

    std::cout << "EventHeader:\n";
    event_header_->Print();
    std::cout << "TofHits:\n";
    tof_->Print();
    std::cout << "VtxTracks:\n";
    vtx_->Print();
    std::cout << "SimParticles:\n";
    sim_->Print();
    std::cout << "TrdTracks:\n";
    trd_->Print();
}

void MlTreePlainer::SetConfig(MLTPConfig::Config config)
{
    json_config_ = config;
}

MLTPConfig::Branch MlTreePlainer::FindBranch(std::string branch_name)
{
    for(auto& b: json_config_.branches)
    {
        if(b.name == branch_name)
        {
            return b;
        }
    }
    return {branch_name, {}};
}