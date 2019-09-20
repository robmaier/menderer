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

#ifndef WIN64
	#define EIGEN_DONT_ALIGN_STATICALLY
#endif
#include <Eigen/Geometry>


namespace menderer
{

    typedef Eigen::Vector2d Vec2;
    typedef Eigen::Vector3d Vec3;
    typedef Eigen::Vector4d Vec4;
    typedef Eigen::Matrix<double, 6, 1> Vec6;
    typedef Eigen::Matrix2d Mat2;
    typedef Eigen::Matrix3d Mat3;
    typedef Eigen::Matrix4d Mat4;
    typedef Eigen::Matrix<double, 6, 6> Mat6;

    typedef Eigen::Vector2f Vec2f;
    typedef Eigen::Vector3f Vec3f;
    typedef Eigen::Vector4f Vec4f;
    typedef Eigen::Matrix<float, 6, 1> Vec6f;
    typedef Eigen::Matrix2f Mat2f;
    typedef Eigen::Matrix3f Mat3f;
    typedef Eigen::Matrix4f Mat4f;
    typedef Eigen::Matrix<float, 6, 6> Mat6f;

    typedef Eigen::Vector2i Vec2i;
    typedef Eigen::Vector3i Vec3i;
    typedef Eigen::Vector4i Vec4i;
    typedef Eigen::Matrix2i Mat2i;
    typedef Eigen::Matrix3i Mat3i;
    typedef Eigen::Matrix4i Mat4i;

    typedef Eigen::Matrix<unsigned int, 2, 1> Vec2ui;
    typedef Eigen::Matrix<unsigned int, 3, 1> Vec3ui;
    typedef Eigen::Matrix<unsigned int, 4, 1> Vec4ui;
    typedef Eigen::Matrix<unsigned int, 5, 1> Vec5ui;
    typedef Eigen::Matrix<unsigned int, 6, 1> Vec6ui;

    typedef Eigen::Matrix<unsigned char, 2, 1> Vec2b;
    typedef Eigen::Matrix<unsigned char, 3, 1> Vec3b;
    typedef Eigen::Matrix<unsigned char, 4, 1> Vec4b;
    typedef Eigen::Matrix<unsigned char, 2, 2> Mat2b;
    typedef Eigen::Matrix<unsigned char, 3, 3> Mat3b;
    typedef Eigen::Matrix<unsigned char, 4, 4> Mat4b;

} // namespace menderer
