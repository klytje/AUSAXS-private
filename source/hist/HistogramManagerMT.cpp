#include <hist/HistogramManagerMT.h>
#include <hist/DistanceHistogram.h>
#include <hist/CompositeDistanceHistogram.h>
#include <hist/detail/CompactCoordinates.h>
#include <data/Protein.h>
#include <data/Atom.h>
#include <data/Water.h>
#include <settings/HistogramSettings.h>
#include <settings/GeneralSettings.h>

#include <BS_thread_pool.hpp>

using namespace hist;

HistogramManagerMT::HistogramManagerMT(HistogramManager& hm) : HistogramManager(hm) {}

HistogramManagerMT::~HistogramManagerMT() = default;

std::unique_ptr<DistanceHistogram> HistogramManagerMT::calculate() {return calculate_all();}

std::unique_ptr<CompositeDistanceHistogram> HistogramManagerMT::calculate_all() {
    double width = settings::axes::distance_bin_width;
    Axis axes(0, settings::axes::max_distance, settings::axes::max_distance/width); 

    // create a more compact representation of the coordinates
    // extremely wasteful to calculate this from scratch every time (class is not meant for serial use anyway?)
    hist::detail::CompactCoordinates data_p(protein->get_bodies());
    hist::detail::CompactCoordinates data_h = hist::detail::CompactCoordinates(protein->get_waters());

    //########################//
    // PREPARE MULTITHREADING //
    //########################//
    BS::thread_pool pool(settings::general::threads);
    auto calc_pp = [&data_p, &axes, &width] (unsigned int imin, unsigned int imax) {
        std::vector<double> p_pp(axes.bins, 0);
        for (unsigned int i = imin; i < imax; ++i) {
            for (unsigned int j = i+1; j < data_p.size; ++j) {
                float weight = data_p.data[i].w*data_p.data[j].w;
                float dx = data_p.data[i].x - data_p.data[j].x;
                float dy = data_p.data[i].y - data_p.data[j].y;
                float dz = data_p.data[i].z - data_p.data[j].z;
                float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
                p_pp[dist/width] += 2*weight;
            }
        }
        return p_pp;
    };
    auto calc_hh = [&data_h, &axes, &width] (unsigned int imin, unsigned int imax) {
        std::vector<double> p_hh(axes.bins, 0);
        for (unsigned int i = imin; i < imax; ++i) {
            for (unsigned int j = i+1; j < data_h.size; ++j) {
                float weight = data_h.data[i].w*data_h.data[j].w;
                float dx = data_h.data[i].x - data_h.data[j].x;
                float dy = data_h.data[i].y - data_h.data[j].y;
                float dz = data_h.data[i].z - data_h.data[j].z;
                float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
                p_hh[dist/width] += 2*weight;
            }
        }
        return p_hh;
    };
    auto calc_hp = [&data_h, &data_p, &axes, &width] (unsigned int imin, unsigned int imax) {
        std::vector<double> p_hp(axes.bins, 0);
        for (unsigned int i = imin; i < imax; ++i) {
            for (unsigned int j = 0; j < data_p.size; ++j) {
                float weight = data_h.data[i].w*data_p.data[j].w;
                float dx = data_h.data[i].x - data_p.data[j].x;
                float dy = data_h.data[i].y - data_p.data[j].y;
                float dz = data_h.data[i].z - data_p.data[j].z;
                float dist = std::sqrt(dx*dx + dy*dy + dz*dz);
                p_hp[dist/width] += 2*weight;
            }
        }
        return p_hp;
    };

    //##############//
    // SUBMIT TASKS //
    //##############//
    unsigned int job_size = settings::general::detail::job_size;
    BS::multi_future<std::vector<double>> pp;
    for (unsigned int i = 0; i < protein->atom_size(); i+=job_size) {
        pp.push_back(pool.submit(calc_pp, i, std::min(i+job_size, (unsigned int)protein->atom_size())));
    }
    BS::multi_future<std::vector<double>> hh;
    for (unsigned int i = 0; i < protein->water_size(); i+=job_size) {
        hh.push_back(pool.submit(calc_hh, i, std::min(i+job_size, (unsigned int)protein->water_size())));
    }
    BS::multi_future<std::vector<double>> hp;
    for (unsigned int i = 0; i < protein->water_size(); i+=job_size) {
        hp.push_back(pool.submit(calc_hp, i, std::min(i+job_size, (unsigned int)protein->water_size())));
    }

    //#################//
    // COLLECT RESULTS //
    //#################//
    auto p_pp_future = pool.submit(
        [&](){
            std::vector<double> p_pp(axes.bins, 0);
            for (const auto& tmp : pp.get()) {
                std::transform(p_pp.begin(), p_pp.end(), tmp.begin(), p_pp.begin(), std::plus<double>());
            }
            return p_pp;
        }
    );

    auto p_hp_future = pool.submit(
        [&](){
            std::vector<double> p_hp(axes.bins, 0);
            for (const auto& tmp : hp.get()) {
                std::transform(p_hp.begin(), p_hp.end(), tmp.begin(), p_hp.begin(), std::plus<double>());
            }
            return p_hp;
        }
    );

    auto p_hh_future = pool.submit(
        [&](){
            std::vector<double> p_hh(axes.bins, 0);
            for (const auto& tmp : hh.get()) {
                std::transform(p_hh.begin(), p_hh.end(), tmp.begin(), p_hh.begin(), std::plus<double>());
            }
            return p_hh;            
        }
    );

    pool.wait_for_tasks();
    auto p_pp = p_pp_future.get();
    auto p_hp = p_hp_future.get();
    auto p_hh = p_hh_future.get();

    //###################//
    // SELF-CORRELATIONS //
    //###################//
    for (unsigned int i = 0; i < protein->atom_size(); ++i) {p_pp[0] += std::pow(data_p.data[i].w, 2);}
    for (unsigned int i = 0; i < protein->water_size(); ++i) {p_hh[0] += std::pow(data_h.data[i].w, 2);}

    // downsize our axes to only the relevant area
    unsigned int max_bin = 10; // minimum size is 10
    for (int i = axes.bins-1; i >= 10; i--) {
        if (p_pp[i] != 0 || p_hh[i] != 0 || p_hp[i] != 0) {
            max_bin = i+1; // +1 since we usually use this for looping (i.e. i < max_bin)
            break;
        }
    }
    p_pp.resize(max_bin);
    p_hh.resize(max_bin);
    p_hp.resize(max_bin);
    axes = Axis(0, max_bin*width, max_bin); 

    // calculate p_tot    
    std::vector<double> p_tot(max_bin, 0);
    for (unsigned int i = 0; i < max_bin; ++i) {p_tot[i] = p_pp[i] + p_hh[i] + 2*p_hp[i];}

    return std::make_unique<CompositeDistanceHistogram>(std::move(p_pp), std::move(p_hp), std::move(p_hh), std::move(p_tot), axes);
}
