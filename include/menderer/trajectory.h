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

#include <vector>


namespace menderer
{

    /**
     * @brief   Container class for camera trajectory.
     * @author  Robert Maier
     */
    class Trajectory
    {
    public:

        /// Constructor that creates an empty trajectory.
        Trajectory();

        /// Constructor that loads a camera trajectory from file.
        Trajectory(const std::string &filename);

        /// Destructor.
        ~Trajectory();

        /**
         * @brief   Loads a camera trajectory from file.
         *          If the file contains only 4 lines, a single pose will only
         *          be loaded. Otherwise, it will treat each line as a single pose
         *          (TUM RGB-D benchmark format).
         */
        bool load(const std::string &filename);

        /// Loads a camera trajectory from multiple files with 4x4 matrices each.
        bool load(const std::vector<std::string> &filenames);

        /// Aligns trajectory to origin, such that initial pose is identity pose.
        void alignToOrigin();

        /// Clears camera trajectory.
        void clear();

        /// Prints out all poses of camera trajectory.
        void print() const;

        /// Returns the number of poses in the camera trajectory.
        size_t size() const;

        /// Checks whether trajectory is empty.
        bool empty() const;

        /// Returns a specific pose in the camera trajectory.
        Mat4 pose(size_t id) const;

    protected:
        /// Counts the number of lines in a text file.
        size_t countLines(const std::string &filename) const;

        /// Loads a camera trajectory from a file in TUM RGB-D benchmark format.
        bool loadFileTrajectoryTUM(const std::string &filename);
        /// Load a single specific camera pose from a text file with 4 lines.
        bool loadFile4x4(const std::string &filename);

        std::vector<Mat4> poses_cam_to_world_;
    };

} // namespace menderer
