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

#include <menderer/mesh.h>

#include <iostream>

namespace menderer
{

    void Mesh::clear()
    {
        vertices.clear();
        normals.clear();
        colors.clear();
        face_vertices.clear();
    }


    bool Mesh::empty() const
    {
        return vertices.empty() || face_vertices.empty();
    }


    void Mesh::print() const
    {
        std::cout << "mesh data:" << std::endl;
        std::cout << "   vertices: " << vertices.size() << std::endl;
        std::cout << "   normals: " << normals.size() << std::endl;
        std::cout << "   colors: " << colors.size() << std::endl;
        std::cout << "   faces: " << face_vertices.size() << std::endl;
    }

} // namespace menderer
