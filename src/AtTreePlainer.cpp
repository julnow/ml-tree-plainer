#include "AtTreePlainer.hpp"
#include "AnalysisTree/TaskManager.hpp"

namespace at = AnalysisTree;

void AtTreePlainer::Init()
{
    auto* man = at::TaskManager::GetInstance();
    auto* chain = man->GetChain();

    {
        auto tofhits_ptr = chain->GetPointerToBranch("TofHits");
        auto simulated_ptr = chain->GetPointerToBranch("SimParticles");
        auto vtxtracks_ptr = chain->GetPointerToBranch("VtxTracks");
        auto rec_event_header_ptr = chain->GetPointerToBranch("RecEventHeader");
        auto trdtracks_ptr = chain->GetPointerToBranch("TrdTracks");

        tofhits_ = ANALYSISTREE_UTILS_GET<at::Detector<at::Hit>*>(tofhits_ptr);
        simulated_ = ANALYSISTREE_UTILS_GET<at::Particles*>(simulated_ptr);
        vtxtracks_ = ANALYSISTREE_UTILS_GET<at::Detector<at::Track>*>(vtxtracks_ptr);
        rec_event_header_ = ANALYSISTREE_UTILS_GET<at::EventHeader*>(rec_event_header_ptr);
        trdtracks_ = ANALYSISTREE_UTILS_GET<at::Detector<at::Track>*>(trdtracks_ptr);
    }

    {
        auto tof2sim_name = config_->GetMatchName("TofHits", "SimParticles");
        auto vtx2tof_name = config_->GetMatchName("VtxTracks", "TofHits");
        auto vtx2sim_name = config_->GetMatchName("VtxTracks", "SimParticles");
        auto vtx2trd_name = config_->GetMatchName("VtxTracks", "TrdTracks");

        auto match_ptrs = chain->GetMatchPointers();

        tof2sim_match_ = match_ptrs.find(tof2sim_name)->second;
        vtx2tof_match_ = match_ptrs.find(vtx2tof_name)->second;
        vtx2sim_match_ = match_ptrs.find(vtx2sim_name)->second;
        vtx2trd_match_ = match_ptrs.find(vtx2trd_name)->second;
    }

    auto out_config = at::TaskManager::GetInstance()->GetConfig();
    at::BranchConfig out_particles(json_config_.output_branch_name, at::DetType::kParticle);

    {
        auto event_header_config = FindBranch("RecEventHeader");
        auto tof_config = FindBranch("TofHits");
        auto vtx_config = FindBranch("VtxTracks");
        auto sim_config = FindBranch("SimParticles");
        auto trd_config = FindBranch("TrdTracks");

        event_header_ = new ATTP::Branch(event_header_config, config_, out_particles);
        tof_ = new ATTP::Branch(tof_config, config_, out_particles);
        vtx_ = new ATTP::Branch(vtx_config, config_, out_particles);
        sim_ = new ATTP::Branch(sim_config, config_, out_particles);
        trd_ = new ATTP::Branch(trd_config, config_, out_particles);
    }

    event_header_->AddFields();
    tof_->AddFields();
    vtx_->AddFields();
    sim_->AddFields();
    trd_->AddFields();

    man->AddBranch(plain_branch_, out_particles);

    InitIndices();
}

void AtTreePlainer::Exec()
{
    plain_branch_->ClearChannels();

    auto out_config = at::TaskManager::GetInstance()->GetConfig();

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

void AtTreePlainer::InitIndices()
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

ATTPConfig::Branch AtTreePlainer::FindBranch(std::string branch_name)
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