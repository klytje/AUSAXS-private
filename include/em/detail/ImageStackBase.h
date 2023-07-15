#pragma once

#include <hist/ScatteringHistogram.h>
#include <em/detail/header/MapHeader.h>
#include <em/Image.h>

#include <vector>
#include <string>
#include <memory>

class Protein;
namespace io {class ExistingFile;}
namespace fitter{class EMFit;}
namespace em {
    class ObjectBounds3D;
    namespace managers {class ProteinManager;}
    class EMHistogramManager;

    /**
     * @brief A representation of a stack of images. 
     */
    class ImageStackBase {
        public:
            /**
             * @brief Constructor.
             * 
             * @param file Path to the input EM data file. 
             */
            ImageStackBase(const io::ExistingFile& file);

            /**
             * @brief Constructor.
             * 
             * @param images The images for this stack.
             */
            ImageStackBase(const std::vector<Image>& images);

            /**
             * @brief Destructor.
             */
            virtual ~ImageStackBase();

            /**
             * @brief Get a specific Image stored in this object. 
             * 
             * @param layer The vertical location of the Image. 
             */
            Image& image(unsigned int layer);

            /**
             * @brief Get a specific Image stored in this object. 
             * 
             * @param layer The vertical location of the Image. 
             */
            const Image& image(unsigned int layer) const;

            /**
             * @brief Prepare a ScatteringHistogram based on this object. 
             */
            hist::ScatteringHistogram get_histogram(double cutoff) const;

            /**
             * @brief Count the number of voxels for a given cutoff.
             */
            unsigned int count_voxels(double cutoff) const;

            /**
             * @brief Get the fitted ScatteringHistogram.
             */
            hist::ScatteringHistogram get_histogram(const std::shared_ptr<fitter::EMFit> res) const;

            /**
             * @brief Get the protein generated with the chosen cutoff value.
             */
            Protein* get_protein(double cutoff) const;

            /**
             * @brief Get the header of the input file. 
             */
            detail::header::MapHeader* get_header() const;

            /**
             * @brief Set the header. 
             */
            void set_header(std::unique_ptr<detail::header::MapHeader> header);

            /**
             * @brief Get the number of images stored in this object.
             */
            unsigned int size() const;

            /**
             * @brief Get a reference to all images stored in this object. 
             */
            const std::vector<Image>& images() const;

            /**
             * @brief Save this structure as a .pdb file. 
             * 
             * @param cutoff The cutoff value. If positive, atoms will be generated at all pixel values higher than this. If negative, they will be generated at pixels lower than this. 
             * @param path Path to save location.
             */
            void save(double cutoff, std::string path) const;

            /**
             * @brief Get the mean density.
             */
            double mean() const;

            em::ObjectBounds3D minimum_volume(double cutoff);

            /**
             * @brief Get the cutoff corresponding to a PyMOL level. This is just the number of sigmas of the root-mean-square deviation.
             */
            double from_level(double sigma) const;

            /**
             * @brief Get the PyMOL level corresponding to a given cutoff. 
             */
            double to_level(double cutoff) const;

            /**
             * @brief Calculate the root-mean-square of this map. 
             */
            double rms() const;

            /**
             * @brief Get the histogram manager.
             */
            em::managers::ProteinManager* get_protein_manager() const;

            /**
             * @brief Determines the minimum bounds necessariy to describe the map for the given cutoff.
             * 
             * @param min_val The smallest possible value. Must be positive.
             * TODO Determine a better, more dynamic approach to determining this minimum. 
             */
            void set_minimum_bounds(double min_val);

        private:
            std::unique_ptr<detail::header::MapHeader> header;  // The header of the input file.
            std::unique_ptr<em::managers::ProteinManager> phm;  // The histogram manager. Manages both the backing protein & its scattering curve. 
            std::vector<Image> data;                            // The actual image data. 
            unsigned int size_x, size_y, size_z;                // The number of pixels in each dimension.
            mutable double _rms = 0;                            // The root-mean-square of the map.
            
            void read(std::ifstream& istream);

            float& index(unsigned int x, unsigned int y, unsigned int z);

            float index(unsigned int x, unsigned int y, unsigned int z) const;
    };
}