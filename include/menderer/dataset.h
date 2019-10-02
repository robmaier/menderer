/**
* This file is part of Menderer.
*
* Copyright 2019 Robert Maier, Technical University of Munich.
* For more information see <https://github.com/robmaier/menderer>.
*
* Menderer is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Menderer is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Menderer. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <menderer/mat.h>

#include <string>
#include <vector>
#include <opencv2/core.hpp>

#include <menderer/camera.h>
#include <menderer/trajectory.h>

namespace menderer
{

    /**
     * @brief   Dataset class.
     *          Mainly for storing camera intrinsics and 
     *          camera trajectory for rendering a scene.
     * @author  Robert Maier
     */
    class Dataset
    {
    public:

        /// Constructor that creates an empty dataset.
        Dataset();

        /// Destructor.
        ~Dataset();

        /// Load poses and intrinsics from separate text files.
        bool load(const std::string &cam_intrinsics_file,
                  const std::string &trajectory_file);

        /// Load poses, intrinsics, color and depth from Intrinsic3D dataset.
        bool load(const std::string &dataset_folder);

        /// Returns the dataset's pinhole camera model.
        const Camera& camera() const;

        /// Returns the dataset's camera poses.
        const Trajectory& trajectory() const;

        /// Checks whether the dataset contains color images.
        bool hasColor() const;

        /// Checks whether the dataset contains depth images.
        bool hasDepth() const;

        /// Returns the number of camera poses/frames in the dataset.
        size_t frames() const;

        /// Load a color image from disk (Intrinsic3D format).
        cv::Mat loadColor(const size_t id) const;

        /// Load a depth map from disk (Intrinsic3D format).
        cv::Mat loadDepth(const size_t id) const;

        /// Save a color image to disk (Intrinsic3D format).
        static bool saveColor(const std::string &filename, const cv::Mat& color);

        /// Save a depth map as .png file (Intrinsic3D format).
        static bool saveDepthPNG(const std::string &filename, const cv::Mat& depth);

        /// Save a depth map as binary file.
        static bool saveDepthBinary(const std::string &filename, const cv::Mat& depth);

        /// Compute a 3D vertex map from a depth map using the camera intrinsics.
        bool depthToVertexMap(const cv::Mat &depth, cv::Mat &vertexMap) const;

    private:
        /// Retrieves the files of an Intrinsic3D dataset.
        bool listFiles(const std::string &dataset_folder,
                       std::vector<std::string> &files_depth,
                       std::vector<std::string> &files_color,
                       std::vector<std::string> &files_poses) const;

        Camera camera_;
        Trajectory trajectory_;

        std::vector<std::string> files_color_;
        std::vector<std::string> files_depth_;
    };

} // namespace menderer
