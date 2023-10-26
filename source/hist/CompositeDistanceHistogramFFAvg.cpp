#include <hist/CompositeDistanceHistogramFFAvg.h>
#include <hist/CompositeDistanceHistogram.h>
#include <hist/Histogram.h>
#include <table/ArrayDebyeTable.h>
#include <form_factor/FormFactor.h>
#include <form_factor/PrecalculatedFormFactorProduct.h>
#include <settings/HistogramSettings.h>

using namespace hist;

CompositeDistanceHistogramFFAvg::CompositeDistanceHistogramFFAvg() = default;

CompositeDistanceHistogramFFAvg::CompositeDistanceHistogramFFAvg(container::Container3D<double>&& p_aa, container::Container2D<double>&& p_aw, container::Container1D<double>&& p_ww, std::vector<double>&& p_tot, const Axis& axis) 
    : CompositeDistanceHistogram(std::move(p_tot), axis), cp_aa(std::move(p_aa)), cp_aw(std::move(p_aw)), cp_ww(std::move(p_ww)) {}

CompositeDistanceHistogramFFAvg::CompositeDistanceHistogramFFAvg(CompositeDistanceHistogramFFAvg&& other) noexcept 
    : CompositeDistanceHistogram(std::move(other.p_aa), std::move(other.p_aw), std::move(other.p_ww), std::move(other.p), other.axis), cw(other.cw), cx(other.cx), 
        cp_aa(std::move(other.cp_aa)), cp_aw(std::move(other.cp_aw)), cp_ww(std::move(other.cp_ww)) {}

CompositeDistanceHistogramFFAvg::~CompositeDistanceHistogramFFAvg() = default;

// #define DEBUG_DEBYE_TRANSFORM 1
// #define DEBUG_PLOT_FF 1
// #if DEBUG_DEBYE_TRANSFORM
//     #include <plots/PlotDataset.h>
//     #include <settings/GeneralSettings.h>
//     #include <dataset/SimpleDataset.h>
//     static unsigned int qcheck = -1;
// #endif
// ScatteringProfile CompositeDistanceHistogramFFAvg::debye_transform() const {
//     const auto& ff_table = form_factor::storage::get_precalculated_form_factor_table();
//     const auto& sinqd_table = table::ArrayDebyeTable::get_default_table();

//     // calculate the Debye scattering intensity
//     Axis debye_axis = constants::axes::q_axis.sub_axis(settings::axes::qmin, settings::axes::qmax);
//     unsigned int q0 = constants::axes::q_axis.get_bin(settings::axes::qmin); // account for a possibly different qmin

//     std::vector<double> Iq(debye_axis.bins, 0);
//     std::vector<double> q_axis = debye_axis.as_vector();

//     #ifdef DEBUG_PLOT_FF
//         std::vector<double> I_aa(q_axis.size(), 0);
//         std::vector<double> I_aw(q_axis.size(), 0);
//         std::vector<double> I_ax(q_axis.size(), 0);
//         std::vector<double> I_ww(q_axis.size(), 0);
//         std::vector<double> I_wx(q_axis.size(), 0);
//         std::vector<double> I_xx(q_axis.size(), 0);
//     #endif

//     unsigned int ff_exv_index = static_cast<int>(form_factor::form_factor_t::EXCLUDED_VOLUME);
//     unsigned int ff_water_index = static_cast<int>(form_factor::form_factor_t::O);
//     for (unsigned int q = q0; q < q0+debye_axis.bins; ++q) {
//         for (unsigned int ff1 = 0; ff1 < form_factor::get_count_without_excluded_volume(); ++ff1) {
//             // atom-atom
//             for (unsigned int ff2 = 0; ff2 < form_factor::get_count_without_excluded_volume(); ++ff2) {
//                 double atom_atom_sum = std::inner_product(cp_aa.begin(ff1, ff2), cp_aa.end(ff1, ff2), sinqd_table.begin(q), 0.0);
//                 Iq[q] += atom_atom_sum*ff_table.index(ff1, ff2).evaluate(q);
//                 #if DEBUG_DEBYE_TRANSFORM
//                     if (q==qcheck && atom_atom_sum != 0) {
//                         std::cout << "(aa) Iq[" << q << "] += aa_sum*ff_table[" << ff1 << ", " << ff2 << "](" << q << ") = " 
//                             << atom_atom_sum << "*" << ff_table.index(ff1, ff2).evaluate(q) << " = " 
//                             << atom_atom_sum*ff_table.index(ff1, ff2).evaluate(q) << std::endl;
//                         for (unsigned int d = 0; d < axis.bins; ++d) {
//                             if (cp_aa.index(ff1, ff2, d) != 0) {std::cout << "\t\taa_sum += p_pp[" << ff1 << ", " << ff2 << ", " << d << "] = " << cp_aa.index(ff1, ff2, d) << "*" << sinqd_table.lookup(q, d) << std::endl;}
//                         }
//                         std::cout << "\taa_sum = " << atom_atom_sum << std::endl;
//                     }
//                     if (q == qcheck && atom_atom_sum != 0) {std::cout << "\tIq[" << q << "] = " << Iq[q] << std::endl;}
//                 #endif
//                 #if DEBUG_PLOT_FF
//                     I_aa[q] += atom_atom_sum*ff_table.index(ff1, ff2).evaluate(q);
//                 #endif
//             }

//             // atom-exv
//             double atom_exv_sum = std::inner_product(cp_aa.begin(ff1, ff_exv_index), cp_aa.end(ff1, ff_exv_index), sinqd_table.begin(q), 0.0);
//             Iq[q] -= 2*cx*atom_exv_sum*ff_table.index(ff1, ff_exv_index).evaluate(q);
//             #if DEBUG_DEBYE_TRANSFORM
//                 if (q==qcheck && atom_exv_sum != 0) {
//                     std::cout << "(ae) Iq[" << q << "] -= 2*ax_sum*ff_table[" << ff1 << ", " << ff_exv_index << "](" << q << ") = " 
//                         << "2*" << atom_exv_sum << "*" << ff_table.index(ff1, ff_exv_index).evaluate(q) << " = " 
//                         << 2*atom_exv_sum*ff_table.index(ff1, ff_exv_index).evaluate(q) << std::endl;
//                     for (unsigned int d = 0; d < axis.bins; ++d) {
//                         if (cp_aa.index(ff1, ff_exv_index, d) != 0) {std::cout << "\t\tax_sum += 2*p_pp[" << ff1 << ", " << ff_exv_index << ", " << d << "] = " << 2*(cp_aa.index(ff1, ff_exv_index, d) + cp_aa.index(ff_exv_index, ff1, d)) << "*" << sinqd_table.lookup(q, d) << std::endl;}
//                     }
//                     std::cout << "\tax_sum = " << atom_exv_sum << std::endl;
//                 }
//                 if (q == qcheck && atom_exv_sum != 0) {std::cout << "\tIq[" << q << "] = " << Iq[q] << std::endl;}
//             #endif
//             #if DEBUG_PLOT_FF
//                 I_ax[q] -= 2*cx*atom_exv_sum*ff_table.index(ff1, ff_exv_index).evaluate(q);
//             #endif

//             // atom-water
//             double atom_water_sum = std::inner_product(cp_aw.begin(ff1), cp_aw.end(ff1), sinqd_table.begin(q), 0.0);
//             Iq[q] += 2*cw*atom_water_sum*ff_table.index(ff1, ff_water_index).evaluate(q);
//             #if DEBUG_DEBYE_TRANSFORM
//                 if (q==qcheck && ff1 == 1) {
//                     std::cout << "(aw) Iq[" << q << "] += 2*aw_sum*ff_table[" << ff1 << ", " << ff_water_index << "](" << q << ") = " 
//                         << "2*" << atom_water_sum << "*" << ff_table.index(ff1, ff_water_index).evaluate(q) 
//                         << 2*atom_water_sum*ff_table.index(ff1, ff_water_index).evaluate(q) << std::endl;
//                     for (unsigned int d = 0; d < axis.bins; ++d) {
//                         if (cp_aw.index(ff1, d) != 0) {std::cout << "\t\taw_sum += p_hp[" << ff1 << ", " << d << "] = " << cp_aw.index(ff1, d) << "*" << sinqd_table.lookup(q, d) << " = " << cp_aw.index(ff1, d)*sinqd_table.lookup(q, d) << std::endl;}
//                     }
//                     std::cout << "\taw_sum = " << atom_water_sum << std::endl;
//                 }
//                 if (q == qcheck && ff1 == 1) {std::cout << "\tIq[" << q << "] = " << Iq[q] << std::endl;}
//             #endif
//             #if DEBUG_PLOT_FF
//                 I_aw[q] += 2*cw*atom_water_sum*ff_table.index(ff1, ff_water_index).evaluate(q);
//             #endif
//         }

//         // exv-exv
//         double exv_exv_sum = std::inner_product(cp_aa.begin(ff_exv_index, ff_exv_index), cp_aa.end(ff_exv_index, ff_exv_index), sinqd_table.begin(q), 0.0);
//         Iq[q] += cx*cx*exv_exv_sum*ff_table.index(ff_exv_index, ff_exv_index).evaluate(q);
//         #if DEBUG_DEBYE_TRANSFORM
//             if (q==qcheck) {
//                 std::cout << "(ee) Iq[" << q << "] += xx_sum*ff_table[" << ff_exv_index << ", " << ff_exv_index << "](" << q << ") = " 
//                     << exv_exv_sum << "*" << ff_table.index(ff_exv_index, ff_exv_index).evaluate(q) << " = " 
//                     << exv_exv_sum*ff_table.index(ff_exv_index, ff_exv_index).evaluate(q) << std::endl;
//                 for (unsigned int d = 0; d < axis.bins; ++d) {
//                     if (cp_aa.index(ff_exv_index, ff_exv_index, d) != 0) {std::cout << "\t\txx_sum += p_pp[" << ff_exv_index << ", " << ff_exv_index << ", " << d << "] = " << cp_aa.index(ff_exv_index, ff_exv_index, d) << std::endl;}
//                 }
//                 std::cout << "\txx_sum = " << exv_exv_sum << std::endl;
//             }
//             if (q == qcheck) {std::cout << "\tIq[" << q << "] = " << Iq[q] << std::endl;}
//         #endif
//         #if DEBUG_PLOT_FF
//             I_xx[q] += cx*cx*exv_exv_sum*ff_table.index(ff_exv_index, ff_exv_index).evaluate(q);
//         #endif

//         // exv-water
//         double exv_water_sum = std::inner_product(cp_aw.begin(ff_exv_index), cp_aw.end(ff_exv_index), sinqd_table.begin(q), 0.0);
//         Iq[q] -= 2*cx*cw*exv_water_sum*ff_table.index(ff_exv_index, ff_water_index).evaluate(q);
//         #if DEBUG_DEBYE_TRANSFORM
//             if (q==qcheck) {
//                 std::cout << "(ew) Iq[" << q << "] -= 2*wx_sum*ff_table[" << ff_exv_index << ", " << ff_water_index << "](" << q << ") = "
//                      << "2*" << exv_water_sum << "*" << ff_table.index(ff_exv_index, ff_water_index).evaluate(q) << " = " 
//                      << 2*exv_water_sum*ff_table.index(ff_exv_index, ff_water_index).evaluate(q) << std::endl;
//                 for (unsigned int d = 0; d < axis.bins; ++d) {
//                     if (cp_aw.index(ff_exv_index, d) != 0) {std::cout << "\t\twx_sum += p_hp[" << ff_exv_index << ", " << d << "] = " << cp_aw.index(ff_exv_index, d) << "*" << sinqd_table.lookup(q, d) << std::endl;}
//                 }
//                 std::cout << "\twx_sum = " << exv_water_sum << std::endl;
//             }
//             if (q == qcheck) {std::cout << "\tIq[" << q << "] = " << Iq[q] << std::endl;}
//         #endif
//         #if DEBUG_PLOT_FF
//             I_wx[q] -= 2*cx*cw*exv_water_sum*ff_table.index(ff_exv_index, ff_water_index).evaluate(q);
//         #endif

//         // water-water
//         double water_water_sum = std::inner_product(cp_ww.begin(), cp_ww.end(), sinqd_table.begin(q), 0.0);
//         Iq[q] += cw*cw*water_water_sum*ff_table.index(ff_water_index, ff_water_index).evaluate(q);
//         #if DEBUG_DEBYE_TRANSFORM
//             if (q==qcheck) {std::cout << "(ww) Iq[" << q << "] += ww_sum*ff_table[" << ff_water_index << ", " << ff_water_index << "](" << q << ") = " 
//                 << water_water_sum << "*" << ff_table.index(ff_water_index, ff_water_index).evaluate(q) << " = " 
//                 << water_water_sum*ff_table.index(ff_water_index, ff_water_index).evaluate(q) << std::endl;}
//             if (q == qcheck) {std::cout << "\tIq[" << q << "] = " << Iq[q] << std::endl;}
//         #endif
//         #if DEBUG_PLOT_FF
//             I_ww[q] += cw*cw*water_water_sum*ff_table.index(ff_water_index, ff_water_index).evaluate(q);
//         #endif
//     }

//     #if DEBUG_PLOT_FF
//         for (unsigned int i = 0; i < q_axis.size(); ++i) {
//             I_aa[i] = std::abs(I_aa[i]);
//             I_aw[i] = std::abs(I_aw[i]);
//             I_ax[i] = std::abs(I_ax[i]);
//             I_ww[i] = std::abs(I_ww[i]);
//             I_wx[i] = std::abs(I_wx[i]);
//             I_xx[i] = std::abs(I_xx[i]);
//         }
//         SimpleDataset temp_aa(q_axis, I_aa);
//         SimpleDataset temp_aw(q_axis, I_aw);
//         SimpleDataset temp_ax(q_axis, I_ax);
//         SimpleDataset temp_ww(q_axis, I_ww);
//         SimpleDataset temp_wx(q_axis, I_wx);
//         SimpleDataset temp_xx(q_axis, I_xx);
//         temp_aa.add_plot_options({{"xlabel", "q"}, {"linewidth", 2}, {"logx", true}, {"logy", true}, {"ylabel", "I"}, {"legend", "I_aa"}, {"color", style::color::red}});
//         temp_aw.add_plot_options({{"xlabel", "q"}, {"linewidth", 2}, {"logx", true}, {"logy", true}, {"ylabel", "I"}, {"legend", "I_aw"}, {"color", style::color::green}});
//         temp_ax.add_plot_options({{"xlabel", "q"}, {"linewidth", 2}, {"logx", true}, {"logy", true}, {"ylabel", "I"}, {"legend", "I_ax"}, {"color", style::color::blue}});
//         temp_ww.add_plot_options({{"xlabel", "q"}, {"linewidth", 2}, {"logx", true}, {"logy", true}, {"ylabel", "I"}, {"legend", "I_ww"}, {"color", style::color::brown}});
//         temp_wx.add_plot_options({{"xlabel", "q"}, {"linewidth", 2}, {"logx", true}, {"logy", true}, {"ylabel", "I"}, {"legend", "I_wx"}, {"color", style::color::orange}});
//         temp_xx.add_plot_options({{"xlabel", "q"}, {"linewidth", 2}, {"logx", true}, {"logy", true}, {"ylabel", "I"}, {"legend", "I_xx"}, {"color", style::color::purple}});

//         plots::PlotDataset(temp_aa)
//             .plot(temp_ax)
//             .plot(temp_xx)
//         .save(settings::general::output + "ff.png");
//         temp_aa.save(settings::general::output + "ff_aa.dat");
//         temp_ax.save(settings::general::output + "ff_ax.dat");
//         temp_xx.save(settings::general::output + "ff_xx.dat");
//         temp_aw.save(settings::general::output + "ff_aw.dat");
//         temp_wx.save(settings::general::output + "ff_wx.dat");
//         temp_ww.save(settings::general::output + "ff_ww.dat");
//     #endif

//     return ScatteringProfile(Iq, debye_axis);
// }

ScatteringProfile CompositeDistanceHistogramFFAvg::debye_transform() const {
    const auto& ff_table = form_factor::storage::get_precalculated_form_factor_table();
    const auto& sinqd_table = table::ArrayDebyeTable::get_default_table();

    // calculate the Debye scattering intensity
    Axis debye_axis = constants::axes::q_axis.sub_axis(settings::axes::qmin, settings::axes::qmax);
    unsigned int q0 = constants::axes::q_axis.get_bin(settings::axes::qmin); // account for a possibly different qmin

    std::vector<double> Iq(debye_axis.bins, 0);
    std::vector<double> q_axis = debye_axis.as_vector();

    unsigned int ff_exv_index = static_cast<int>(form_factor::form_factor_t::EXCLUDED_VOLUME);
    unsigned int ff_water_index = static_cast<int>(form_factor::form_factor_t::O);
    for (unsigned int q = q0; q < q0+debye_axis.bins; ++q) {
        for (unsigned int ff1 = 0; ff1 < form_factor::get_count_without_excluded_volume(); ++ff1) {
            // atom-atom
            for (unsigned int ff2 = 0; ff2 < form_factor::get_count_without_excluded_volume(); ++ff2) {
                double aa_sum = std::inner_product(cp_aa.begin(ff1, ff2), cp_aa.end(ff1, ff2), sinqd_table.begin(q), 0.0);
                Iq[q] += aa_sum*ff_table.index(ff1, ff2).evaluate(q);
            }

            // atom-exv
            double ax_sum = std::inner_product(cp_aa.begin(ff1, ff_exv_index), cp_aa.end(ff1, ff_exv_index), sinqd_table.begin(q), 0.0);
            Iq[q] -= 2*cx*ax_sum*ff_table.index(ff1, ff_exv_index).evaluate(q);

            // atom-water
            double aw_sum = std::inner_product(cp_aw.begin(ff1), cp_aw.end(ff1), sinqd_table.begin(q), 0.0);
            Iq[q] += 2*cw*aw_sum*ff_table.index(ff1, ff_water_index).evaluate(q);
        }

        // exv-exv
        double xx_sum = std::inner_product(cp_aa.begin(ff_exv_index, ff_exv_index), cp_aa.end(ff_exv_index, ff_exv_index), sinqd_table.begin(q), 0.0);
        Iq[q] += cx*cx*xx_sum*ff_table.index(ff_exv_index, ff_exv_index).evaluate(q);

        // exv-water
        double ew_sum = std::inner_product(cp_aw.begin(ff_exv_index), cp_aw.end(ff_exv_index), sinqd_table.begin(q), 0.0);
        Iq[q] -= 2*cx*cw*ew_sum*ff_table.index(ff_exv_index, ff_water_index).evaluate(q);

        // water-water
        double ww_sum = std::inner_product(cp_ww.begin(), cp_ww.end(), sinqd_table.begin(q), 0.0);
        Iq[q] += cw*cw*ww_sum*ff_table.index(ff_water_index, ff_water_index).evaluate(q);
    }
    return ScatteringProfile(Iq, debye_axis);
}

const std::vector<double>& CompositeDistanceHistogramFFAvg::get_counts() const {
    p = std::vector<double>(axis.bins, 0);
    auto& p_pp = get_aa_counts();
    auto& p_hp = get_aw_counts();
    auto& p_hh = get_ww_counts();
    for (unsigned int i = 0; i < axis.bins; ++i) {
        p[i] = p_pp[i] + 2*cw*p_hp[i] + cw*cw*p_hh[i];
    }
    return p.data;
}

const std::vector<double>& CompositeDistanceHistogramFFAvg::get_aa_counts() const {
    p_aa = std::vector<double>(axis.bins, 0);
    for (unsigned int ff1 = 0; ff1 < form_factor::get_count_without_excluded_volume(); ++ff1) {
        for (unsigned int ff2 = 0; ff2 < form_factor::get_count_without_excluded_volume(); ++ff2) {
            std::transform(p_aa.begin(), p_aa.end(), cp_aa.begin(ff1, ff2), p_aa.begin(), std::plus<double>());
        }
    }
    return p_aa;
}

const std::vector<double>& CompositeDistanceHistogramFFAvg::get_aw_counts() const {
    p_aw = std::vector<double>(axis.bins, 0);
    for (unsigned int ff1 = 0; ff1 < form_factor::get_count_without_excluded_volume(); ++ff1) {
        std::transform(p_aw.begin(), p_aw.end(), cp_aw.begin(ff1), p_aw.begin(), std::plus<double>());
    }
    return p_aw;
}

const std::vector<double>& CompositeDistanceHistogramFFAvg::get_ww_counts() const {
    p_ww = std::vector<double>(axis.bins, 0);
    std::transform(p_ww.begin(), p_ww.end(), cp_ww.begin(), p_ww.begin(), std::plus<double>());
    return p_ww;
}

void CompositeDistanceHistogramFFAvg::apply_water_scaling_factor(double k) {
    cw = k;
}

void CompositeDistanceHistogramFFAvg::apply_excluded_volume_scaling_factor(double k) {
    cx = k;
}

const ScatteringProfile CompositeDistanceHistogramFFAvg::get_profile_aa() const {
    const auto& ff_table = form_factor::storage::get_precalculated_form_factor_table();
    const auto& sinqd_table = table::ArrayDebyeTable::get_default_table();
    Axis debye_axis = constants::axes::q_axis.sub_axis(settings::axes::qmin, settings::axes::qmax);
    unsigned int q0 = constants::axes::q_axis.get_bin(settings::axes::qmin); // account for a possibly different qmin

    std::vector<double> Iq(debye_axis.bins, 0);
    for (unsigned int q = q0; q < q0+debye_axis.bins; ++q) {
        for (unsigned int ff1 = 0; ff1 < form_factor::get_count_without_excluded_volume(); ++ff1) {
            for (unsigned int ff2 = 0; ff2 < form_factor::get_count_without_excluded_volume(); ++ff2) {
                double aa_sum = std::inner_product(cp_aa.begin(ff1, ff2), cp_aa.end(ff1, ff2), sinqd_table.begin(q), 0.0);
                Iq[q] += aa_sum*ff_table.index(ff1, ff2).evaluate(q);
            }
        }
    }
    return ScatteringProfile(Iq, debye_axis);
}

const ScatteringProfile CompositeDistanceHistogramFFAvg::get_profile_ax() const {
    const auto& ff_table = form_factor::storage::get_precalculated_form_factor_table();
    const auto& sinqd_table = table::ArrayDebyeTable::get_default_table();
    Axis debye_axis = constants::axes::q_axis.sub_axis(settings::axes::qmin, settings::axes::qmax);
    unsigned int q0 = constants::axes::q_axis.get_bin(settings::axes::qmin); // account for a possibly different qmin

    std::vector<double> Iq(debye_axis.bins, 0);
    unsigned int ff_exv_index = static_cast<int>(form_factor::form_factor_t::EXCLUDED_VOLUME);
    for (unsigned int q = q0; q < q0+debye_axis.bins; ++q) {
        for (unsigned int ff1 = 0; ff1 < form_factor::get_count_without_excluded_volume(); ++ff1) {
            double ax_sum = std::inner_product(cp_aa.begin(ff1, ff_exv_index), cp_aa.end(ff1, ff_exv_index), sinqd_table.begin(q), 0.0);
            Iq[q] += 2*cx*ax_sum*ff_table.index(ff1, ff_exv_index).evaluate(q);
        }
    }
    return ScatteringProfile(Iq, debye_axis);
}

const ScatteringProfile CompositeDistanceHistogramFFAvg::get_profile_xx() const {
    const auto& ff_table = form_factor::storage::get_precalculated_form_factor_table();
    const auto& sinqd_table = table::ArrayDebyeTable::get_default_table();
    Axis debye_axis = constants::axes::q_axis.sub_axis(settings::axes::qmin, settings::axes::qmax);
    unsigned int q0 = constants::axes::q_axis.get_bin(settings::axes::qmin); // account for a possibly different qmin

    std::vector<double> Iq(debye_axis.bins, 0);
    unsigned int ff_exv_index = static_cast<int>(form_factor::form_factor_t::EXCLUDED_VOLUME);
    for (unsigned int q = q0; q < q0+debye_axis.bins; ++q) {
        double xx_sum = std::inner_product(cp_aa.begin(ff_exv_index, ff_exv_index), cp_aa.end(ff_exv_index, ff_exv_index), sinqd_table.begin(q), 0.0);
        Iq[q] += cx*cx*xx_sum*ff_table.index(ff_exv_index, ff_exv_index).evaluate(q);
    }
    return ScatteringProfile(Iq, debye_axis);
}

const ScatteringProfile CompositeDistanceHistogramFFAvg::get_profile_wx() const {
    const auto& ff_table = form_factor::storage::get_precalculated_form_factor_table();
    const auto& sinqd_table = table::ArrayDebyeTable::get_default_table();
    Axis debye_axis = constants::axes::q_axis.sub_axis(settings::axes::qmin, settings::axes::qmax);
    unsigned int q0 = constants::axes::q_axis.get_bin(settings::axes::qmin); // account for a possibly different qmin

    std::vector<double> Iq(debye_axis.bins, 0);
    unsigned int ff_water_index = static_cast<int>(form_factor::form_factor_t::O);
    unsigned int ff_exv_index = static_cast<int>(form_factor::form_factor_t::EXCLUDED_VOLUME);
    for (unsigned int q = q0; q < q0+debye_axis.bins; ++q) {
        double ew_sum = std::inner_product(cp_aw.begin(ff_exv_index), cp_aw.end(ff_exv_index), sinqd_table.begin(q), 0.0);
        Iq[q] += 2*cx*cw*ew_sum*ff_table.index(ff_exv_index, ff_water_index).evaluate(q);
    }
    return ScatteringProfile(Iq, debye_axis);
}

const ScatteringProfile CompositeDistanceHistogramFFAvg::get_profile_aw() const {
    const auto& ff_table = form_factor::storage::get_precalculated_form_factor_table();
    const auto& sinqd_table = table::ArrayDebyeTable::get_default_table();
    Axis debye_axis = constants::axes::q_axis.sub_axis(settings::axes::qmin, settings::axes::qmax);
    unsigned int q0 = constants::axes::q_axis.get_bin(settings::axes::qmin); // account for a possibly different qmin

    std::vector<double> Iq(debye_axis.bins, 0);
    unsigned int ff_water_index = static_cast<int>(form_factor::form_factor_t::O);
    for (unsigned int q = q0; q < q0+debye_axis.bins; ++q) {
        for (unsigned int ff1 = 0; ff1 < form_factor::get_count_without_excluded_volume(); ++ff1) {
            double aw_sum = std::inner_product(cp_aw.begin(ff1), cp_aw.end(ff1), sinqd_table.begin(q), 0.0);
            Iq[q] += 2*cw*aw_sum*ff_table.index(ff1, ff_water_index).evaluate(q);
        }
    }
    return ScatteringProfile(Iq, debye_axis);
}

const ScatteringProfile CompositeDistanceHistogramFFAvg::get_profile_ww() const {
    const auto& ff_table = form_factor::storage::get_precalculated_form_factor_table();
    const auto& sinqd_table = table::ArrayDebyeTable::get_default_table();
    Axis debye_axis = constants::axes::q_axis.sub_axis(settings::axes::qmin, settings::axes::qmax);
    unsigned int q0 = constants::axes::q_axis.get_bin(settings::axes::qmin); // account for a possibly different qmin

    std::vector<double> Iq(debye_axis.bins, 0);
    unsigned int ff_water_index = static_cast<int>(form_factor::form_factor_t::O);
    for (unsigned int q = q0; q < q0+debye_axis.bins; ++q) {
        double ww_sum = std::inner_product(cp_ww.begin(), cp_ww.end(), sinqd_table.begin(q), 0.0);
        Iq[q] += cw*cw*ww_sum*ff_table.index(ff_water_index, ff_water_index).evaluate(q);
    }
    return ScatteringProfile(Iq, debye_axis);
}