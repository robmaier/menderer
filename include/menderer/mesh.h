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
     * @brief   Struct for representing a 3D triangle mesh.
     * @author  Robert Maier
     */
    struct Mesh
    {
    public:

        /// Clear mesh data.
        void clear();

        /// Checks whether mesh is empty.
        bool empty() const;

        /// Print mesh information.
        void print() const;

        std::vector<Vec3> vertices;
        std::vector<Vec3> normals;
        std::vector<Vec3b> colors;
        std::vector<Vec3ui> face_vertices;
    };

} // namespace menderer
