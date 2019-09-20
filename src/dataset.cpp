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

#include <menderer/dataset.h>

#include <iomanip>
#include <iostream>
#include <fstream>

#include <opencv2/highgui.hpp>


namespace menderer
{

    Dataset::Dataset() :
        camera_(),
        trajectory_()
    {
    }


    Dataset::~Dataset()
    {
    }


    const Camera& Dataset::camera() const
    {
        return camera_;
    }


    const Trajectory& Dataset::trajectory() const
    {
        return trajectory_;
    }


    bool Dataset::hasColor() const
    {
        return !files_color_.empty();
    }


    bool Dataset::hasDepth() const
    {
        return !files_depth_.empty();
    }


    size_t Dataset::frames() const
    {
        return trajectory_.size();
    }


    bool Dataset::load(const std::string &cam_intrinsics_file,
                       const std::string &trajectory_file)
    {
        // load camera intrinsics
        bool ok = camera_.load(cam_intrinsics_file);

        // load trajectory
        if (ok)
            ok = trajectory_.load(trajectory_file);

        return ok;
    }


    bool Dataset::load(const std::string &dataset_folder)
    {
        if (dataset_folder.empty())
            return false;

        std::cout << "loading Intrinsic3D dataset ..." << std::endl;

        // load frame filenames
        std::vector<std::string> files_poses;
        if (!listFiles(dataset_folder, files_depth_, files_color_, files_poses))
        {
            std::cerr << "filenames could not be retrieved!" << std::endl;
            return false;
        }
        std::cout << "   " << files_poses.size() << " frames detected." << std::endl;

        bool ok = true;

        // load first frame to get image dimensions
        cv::Mat color0 = loadColor(0);
        ok = !color0.empty();

        // load color camera intrinsics
        if (ok)
        {
            std::string cam_intrinsics_file = dataset_folder + "/colorIntrinsics.txt";
            ok = camera_.load(cam_intrinsics_file, color0.cols, color0.rows);
        }

        // load trajectory
        if (ok)
            ok = trajectory_.load(files_poses);

        return true;
    }


    cv::Mat Dataset::loadColor(const size_t id) const
    {
        if (id >= files_color_.size() || files_color_.empty())
            return cv::Mat();
        return cv::imread(files_color_[id]);
    }


    cv::Mat Dataset::loadDepth(const size_t id) const
    {
        if (id >= files_depth_.size() || files_depth_.empty())
            return cv::Mat();
        // read 16-bit depth image
        cv::Mat depth16 = cv::imread(files_depth_[id], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
        // convert 16-bit depth image to float
        cv::Mat depth;
        depth16.convertTo(depth, CV_32FC1, (1.0 / 1000.0));
        return depth;
    }


    bool Dataset::listFiles(const std::string &dataset_folder,
                            std::vector<std::string> &files_depth,
                            std::vector<std::string> &files_color,
                            std::vector<std::string> &files_poses) const
    {
        if (dataset_folder.empty())
            return false;

        // get frame filenames
        for (size_t i = 0; i < 999999; ++i)
        {
            std::stringstream ss;
            ss << dataset_folder << "/" << "frame-" << std::setfill('0') << std::setw(6) << i;
            std::string filename_base = ss.str();

            // add depth map
            std::string filename_depth = filename_base + ".depth.png";
            if (!std::ifstream(filename_depth.c_str()).is_open())
                break;
            files_depth.push_back(filename_depth);
            // add color image
            files_color.push_back(filename_base + ".color.png");
            // add pose file
            files_poses.push_back(filename_base + ".pose.txt");
        }

        if (files_poses.empty() ||
                files_poses.size() != files_color.size() ||
                files_poses.size() != files_depth.size())
            return false;
        else
            return true;
    }


    bool Dataset::saveColor(const std::string &filename, const cv::Mat& color)
    {
        if (filename.empty() || color.empty())
            return false;

        cv::imwrite(filename, color);
        return true;
    }


    bool Dataset::saveDepthPNG(const std::string &filename, const cv::Mat& depth)
    {
        if (filename.empty() || depth.empty() || depth.type() != CV_32FC1)
            return false;

        // store rendered depth map as .png (16 bit unsigned short)
        cv::Mat depth16;
        depth.convertTo(depth16, CV_16UC1, 5000.0);
        cv::imwrite(filename, depth16);

        return true;
    }


    bool Dataset::saveDepthBinary(const std::string &filename, const cv::Mat& depth)
    {
        if (filename.empty() || depth.empty() || depth.type() != CV_32FC1)
            return false;

        // store float depth to binary file
        std::ofstream out_file;
        out_file.open(filename.c_str(), std::ios::binary);
        if (!out_file.is_open())
            return false;

        // write depth data
        size_t w = static_cast<size_t>(depth.cols);
        size_t h = static_cast<size_t>(depth.rows);
        size_t size = w * h;
        out_file.write((const char*)depth.data, sizeof(float) * size);
        out_file.close();

        return true;
    }

} // namespace menderer
