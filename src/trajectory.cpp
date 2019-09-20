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

#include <menderer/trajectory.h>

#include <fstream>
#include <iostream>


namespace menderer
{

    Trajectory::Trajectory()
    {
    }


    Trajectory::Trajectory(const std::string &filename)
    {
        bool ok = load(filename);
        assert(ok);
    }


    Trajectory::~Trajectory()
    {
    }


    bool Trajectory::load(const std::string &filename)
    {
        if (filename.empty())
            return false;

        poses_cam_to_world_.clear();

        // count lines in file
        size_t num_lines = countLines(filename);
        //std::cout << "# lines: " << num_lines << std::endl;
        if (num_lines == 0)
            return false;

        // load trajectory from file
        bool ok;
        if (num_lines == 4)
        {
            // load file with single 4x4 transformation matrix
            ok = loadFile4x4(filename);
        }
        else
        {
            // load trajectory in TUM RGB-D benchmark format
            ok = loadFileTrajectoryTUM(filename);
        }

        return ok;
    }


    bool Trajectory::load(const std::vector<std::string> &filenames)
    {
        if (filenames.empty())
            return false;

        poses_cam_to_world_.clear();

        for (const auto& filename : filenames)
        {
            // load file with single 4x4 transformation matrix
            loadFile4x4(filename);
        }
        return true;
    }


    void Trajectory::alignToOrigin()
    {
        if (poses_cam_to_world_.empty())
            return;

        // align trajectory to origin,
        // i.e. initial pose is identity pose

        // retrieve initial pose as reference
        Mat4 initial_pose_inv = poses_cam_to_world_[0].inverse();
        // align all poses by applying inverse reference pose
        for (size_t i = 0; i < poses_cam_to_world_.size(); ++i)
            poses_cam_to_world_[i] = initial_pose_inv * poses_cam_to_world_[i];
    }


    size_t Trajectory::size() const
    {
        return poses_cam_to_world_.size();
    }


    bool Trajectory::empty() const
    {
        return poses_cam_to_world_.empty();
    }


    void Trajectory::clear()
    {
        poses_cam_to_world_.clear();
    }


    void Trajectory::print() const
    {
        for (size_t i = 0; i < poses_cam_to_world_.size(); ++i)
        {
            std::cout << "pose " << i << ":" << std::endl;
            std::cout << poses_cam_to_world_[i] << std::endl;
        }
    }


    Mat4 Trajectory::pose(size_t id) const
    {
        assert(id < poses_cam_to_world_.size());
        return poses_cam_to_world_[id];
    }


    size_t Trajectory::countLines(const std::string &filename) const
    {
        size_t num_lines = 0;

        // open file
        std::ifstream file(filename.c_str());
        if (!file.is_open())
            return 0;

        // read file line by line
        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty() || line.compare(0, 1, "#") == 0)
                continue;
            ++num_lines;
        }
        file.close();

        return num_lines;
    }


    bool Trajectory::loadFileTrajectoryTUM(const std::string &filename)
    {
        // open file
        std::ifstream file(filename.c_str());
        if (!file.is_open())
            return false;

        // load all timestamps and poses
        try
        {
            std::string line;
            while (std::getline(file, line))
            {
                if (line.empty() || line.compare(0, 1, "#") == 0)
                    continue;
                std::istringstream iss(line);
                double timestamp;
                double tx, ty, tz;
                double qx, qy, qz, qw;
                if (!(iss >> timestamp >> tx >> ty >> tz >> qx >> qy >> qz >> qw))
                    break;

                // fill pose from translation vector and quaternion in file
                Vec3 translation(tx, ty, tz);
                Eigen::Quaterniond quat(qw, qx, qy, qz);
                Mat4 pose_cam_to_world = Mat4::Identity();
                pose_cam_to_world.topRightCorner(3,1) = translation;
                pose_cam_to_world.topLeftCorner(3,3) = quat.toRotationMatrix();
                poses_cam_to_world_.push_back(pose_cam_to_world);
            }
            file.close();
        }
        catch (...)
        {
            poses_cam_to_world_.clear();
            file.close();
            return false;
        }

        return true;
    }


    bool Trajectory::loadFile4x4(const std::string &filename)
    {
        // open file
        std::ifstream file(filename.c_str());
        if (!file.is_open())
            return false;

        try
        {
            // load pose from file
            Mat4 pose;
            double val = 0.0;
            for (int r = 0; r < 4; r++)
            {
                for (int c = 0; c < 4; c++)
                {
                    file >> val;
                    pose(r, c) = val;
                }
            }
            file.close();

            // store pose
            poses_cam_to_world_.push_back(pose);
        }
        catch (...)
        {
            return false;
        }

        return true;
    }

} // namespace menderer
