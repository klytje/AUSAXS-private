#pragma once

#include <vector>

#include <em/CullingStrategy.h>
#include <em/datatypes.h>
#include <em/Image.h>
#include <data/Protein.h>
#include <hydrate/Grid.h>
#include <ScatteringHistogram.h>
#include <fitter/SimpleIntensityFitter.h>

using std::vector, std::list;

namespace em {
    /**
     * @brief \class ImageStack
     * 
     * A representation of a stack of images. 
     */
    class ImageStack {
        public:
            /**
             * @brief Constructor.
             * 
             * @param file Path to the input EM data file. 
             */
            ImageStack(string file, setting::em::CullingStrategyChoice csc = setting::em::CullingStrategyChoice::CounterStrategy);

            /**
             * @brief Destructor.
             */
            ~ImageStack() = default;

            /**
             * @brief Fit the cutoff value with the input experimental data file. 
             * 
             * @param filename Path to the measurement file. 
             */
            void fit(string filename) const;

            /**
             * @brief Fit the cutoff value with the input histogram. 
             * 
             * @param h The histogram to fit to.  
             */
            void fit(const ScatteringHistogram& h) const;

            /**
             * @brief Get a specific Image stored in this object. 
             * 
             * @param layer The vertical location of the Image. 
             */
            Image& image(unsigned int layer);

            /**
             * @brief Prepare a ScatteringHistogram based on this object. 
             */
            ScatteringHistogram get_histogram(double cutoff) const;

            /**
             * @brief Create a new Grid based on this object. 
             * 
             * @param cutoff The cutoff value. If positive, atoms will be generated at all pixel values higher than this. If negative, they will be generated at pixels lower than this. 
             */
            std::unique_ptr<Grid> create_grid(double cutoff) const;

            /**
             * @brief Create a new Protein based on this object. 
             * 
             * @param cutoff The cutoff value. If positive, atoms will be generated at all pixel values higher than this. If negative, they will be generated at pixels lower than this. 
             */
            std::unique_ptr<Protein> create_protein(double cutoff) const;

            /**
             * @brief Get the header of the input file. 
             */
            std::shared_ptr<ccp4::Header> get_header() const;

            /**
             * @brief Get the number of images stored in this object.
             */
            size_t size() const;

            /**
             * @brief Get a reference to all images stored in this object. 
             */
            const vector<Image>& images() const;

            /**
             * @brief Save this structure as a .pdb file. 
             * 
             * @param path Path to save location.
             * @param cutoff The cutoff value. If positive, atoms will be generated at all pixel values higher than this. If negative, they will be generated at pixels lower than this. 
             */
            void save(string path, double cutoff) const;

        private:
            std::shared_ptr<ccp4::Header> header;
            vector<Image> data;
            std::unique_ptr<em::CullingStrategy> culler;

            void read(std::ifstream& istream, size_t byte_size);

            void setup(setting::em::CullingStrategyChoice csc);

            size_t get_byte_size() const;

            void fit_helper(SimpleIntensityFitter& fitter) const;

            float& index(unsigned int x, unsigned int y, unsigned int z);
            float index(unsigned int x, unsigned int y, unsigned int z) const;
    };
}