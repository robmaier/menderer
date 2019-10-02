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

#include <menderer/camera.h>

#include <iostream>
#include <fstream>


namespace menderer
{

    Camera::Camera() :
        K_(Mat3::Identity()),
        width_(0),
        height_(0)
    {
        reset();
    }


    Camera::Camera(const std::string &filename) :
        K_(Mat3::Identity()),
        width_(0),
        height_(0)
    {
        bool ok = load(filename);
        assert(ok);
    }


    Camera::~Camera()
    {
    }


    int Camera::width() const
    {
        return width_;
    }


    int Camera::height() const
    {
        return height_;
    }


    Mat3 Camera::intrinsics() const
    {
        return K_;
    }


    bool Camera::load(const std::string &filename)
    {
        std::ifstream file(filename.c_str());
        if (!file.is_open())
            return false;

        bool loaded = false;
        try
        {
            // read camera width and height
            file >> width_ >> height_;
            // read camera intrinsics
            double val = 0.0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    file >> val;
                    K_(i, j) = val;
                }
            }
            file.close();
            loaded = true;
        }
        catch (...)
        {
        }

        if (!loaded)
        {
            std::cerr << "camera intrinsics could not be loaded!" << std::endl;
            reset();
        }

        return loaded;
    }


    bool Camera::load(const std::string &filename, int width, int height)
    {
        std::ifstream file(filename.c_str());
        if (!file.is_open())
            return false;

        // set width and height
        width_ = width;
        height_ = height;

        bool loaded = false;
        try
        {
            // read camera intrinsics
            Mat4 K_file;
            double val = 0.0;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    file >> val;
                    K_file(i, j) = val;
                }
            }
            K_ = K_file.topLeftCorner<3,3>();
            file.close();
            loaded = true;
        }
        catch (...)
        {
        }

        if (!loaded)
        {
            std::cerr << "camera intrinsics could not be loaded!" << std::endl;
            reset();
        }

        return loaded;
    }


    void Camera::print() const
    {
        std::cout << "camera intrinsics:" << std::endl;
        std::cout << "   size: " << width_ << "x" << height_ << std::endl;
        std::cout << "   intrinsics: fx=" << K_(0, 0) << ", fy=" << K_(1, 1) <<
                     ", cx=" << K_(0, 2) << ", cy=" << K_(1, 2) << std::endl;
    }


    void Camera::reset()
    {
        // set default camera intrinsics
        K_ <<   525.0, 0.0, 319.5,
                0.0, 525.0, 239.5,
                0.0, 0.0, 1.0;
        width_ = 640;
        height_ = 480;
    }


    bool Camera::project(const Vec3f &pt, Vec2f &pt2f, Vec2i &pt2i) const
    {
        // shorthands
        const float fx = static_cast<float>(K_(0, 0));
        const float fy = static_cast<float>(K_(1, 1));
        const float cx = static_cast<float>(K_(0, 2));
        const float cy = static_cast<float>(K_(1, 2));

        // compute normalized 2D point (project 3d point onto image plane)
        float x = pt[0] / pt[2];
        float y = pt[1] / pt[2];
        // convert point to pixel coordinates and apply center pixel offset
        pt2f[0] = fx * x + cx;
        pt2f[1] = fy * y + cy;
        // compute integer 2D image coordinates (round to nearest integer)
        pt2i = Vec2i(static_cast<int>(pt2f[0] + 0.5f), static_cast<int>(pt2f[1] + 0.5f));
        // check if points is within image bounds
        if (pt2i[0] < 0 || pt2i[0] >= width_ || pt2i[1] < 0 || pt2i[1] >= height_)
            return false;
        return true;
    }


    Vec3f Camera::unproject(int x, int y, float depth) const
    {
        // shorthands
        const float fx_inv = 1.0f / static_cast<float>(K_(0, 0));
        const float fy_inv = 1.0f / static_cast<float>(K_(1, 1));
        const float cx = static_cast<float>(K_(0, 2));
        const float cy = static_cast<float>(K_(1, 2));

        Vec3f pt = Vec3f::Zero();
        if (depth != 0.0f && !std::isnan(depth))
        {
            pt[0] = (float(x) - cx) * fx_inv * depth;
            pt[1] = (float(y) - cy) * fy_inv * depth;
            pt[2] = depth;
        }
        return pt;
    }

} // namespace menderer
