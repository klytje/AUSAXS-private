#pragma once

#include <hist/distance_calculator/HistogramManager.h>

namespace hist {
	namespace detail {class CompactCoordinates;}

	/**
	 * @brief A multi-threaded simple distance calculator. 
     *        This class is only intended for testing. Use the PartialHistogramManagerMT class for production.
	 */
	class HistogramManagerMT : public HistogramManager {
		public:
			using HistogramManager::HistogramManager;

			HistogramManagerMT(HistogramManager&);

			virtual ~HistogramManagerMT() override;

			/**
			 * @brief Calculate only the total scattering histogram. 
			 */
			std::unique_ptr<DistanceHistogram> calculate() override;

			/**
			 * @brief Calculate all contributions to the scattering histogram. 
			 */
			std::unique_ptr<ICompositeDistanceHistogram> calculate_all() override;

		protected:
			std::unique_ptr<hist::detail::CompactCoordinates> data_p_ptr;
		    std::unique_ptr<hist::detail::CompactCoordinates> data_h_ptr;
	};
}