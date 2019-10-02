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
#include <opencv2/core/core.hpp>


namespace menderer
{

    /**
     * @brief   Pinhole camera model class.
     * @author  Robert Maier
     */
    class Camera
    {
    public:

        /**
         * @brief   Constructor for creating a pinhole camera with
         *          default parameters.
         */
        Camera();

        /**
         * @brief   Constructor for creating a pinhole camera with
         *          parameters loaded from a text file.
         */
        Camera(const std::string &filename);

        /// Destructor.
        ~Camera();

        /// Load camera intrinsics from a file.
        bool load(const std::string &filename);

        /// Load camera intrinsics from a file with specific width/height.
        bool load(const std::string &filename, int width, int height);

        /// Get width of the camera/image.
        int width() const;

        /// Get height of the camera/image.
        int height() const;

        /// Get the 3x3 camera intrinsics matrix.
        Mat3 intrinsics() const;

        /// Print pinhole camera model parameters.
        void print() const;

        /// Projects a point from 3D into 2D image.
        bool project(const Vec3f &pt, Vec2f &pt2f, Vec2i &pt2i) const;

        /// Unprojects a 2D image point back to 3D using its depth.
        Vec3f unproject(int x, int y, float depth) const;

    private:
        /// Reset pinhole camera model parameters to default values.
        void reset();

        Mat3 K_;
        int width_;
        int height_;
    };

} // namespace menderer
