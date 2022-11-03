#pragma once

#include <plots/Plot.h>
#include <utility/Dataset.h>
#include <utility/Multiset.h>

#include <memory>
#include <string>
#include <sstream>

namespace plots {
	class PlotDataset : public Plot {
		public:
			/**
			 * @brief Default constructor.
			 */
			PlotDataset() {}

			/**
			 * @brief Constructor.
			 */
			template<typename T>
			PlotDataset(const T& d);

			/**
			 * @brief Constructor.
			 */
			PlotDataset(const Multiset& d);

			/**
			 * @brief Destructor.
			 */
			~PlotDataset() override;

			/**
			 * @brief Plot an additional Dataset. 
			 */
			template<typename T> 
			void plot(const T& data);

			/**
			 * @brief Plot and save the input dataset at the specified location. 
			 * 	      This is a convenient shortcut for quickly creating a plot of a single dataset. 
			 */
			template<typename T>
			static void quick_plot(const T& data, std::string path);
		};
}