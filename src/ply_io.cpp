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

#include <menderer/ply_io.h>

#include <iostream>
#include <array>
#include <vector>


namespace menderer
{

    bool PlyIO::load(const std::string &filename, Mesh &mesh)
    {
        if (filename.empty())
            return false;
        std::ifstream file(filename.c_str(), std::ios::binary);
        if (!file)
            return false;

        try
        {
            // load ply file using happly
            happly::PLYData ply_in(filename);

            // get mesh data
            const auto& vert_positions = ply_in.getVertexPositions();
            const auto& vert_colors = ply_in.getVertexColors();
            const auto& vert_normals = getVertexNormals(ply_in);
            const auto& face_indices = ply_in.getFaceIndices<unsigned int>();

            // vertex positions
            mesh.vertices.resize(vert_positions.size());
            for (size_t i = 0; i < vert_positions.size(); ++i)
            {
                const auto& v = vert_positions[i];
                mesh.vertices[i] = Vec3(v[0], v[1], v[2]);
            }

            // vertex colors
            mesh.colors.resize(vert_colors.size());
            for (size_t i = 0; i < vert_colors.size(); ++i)
            {
                const auto& c = vert_colors[i];
                mesh.colors[i] = Vec3b(c[0], c[1], c[2]);
            }

            // vertex normals
            mesh.normals.resize(vert_normals.size());
            for (size_t i = 0; i < vert_normals.size(); ++i)
            {
                const auto& n = vert_normals[i];
                mesh.normals[i] = Vec3(n[0], n[1], n[2]);
            }

            // faces
            mesh.face_vertices.resize(face_indices.size());
            for (size_t i = 0; i < face_indices.size(); ++i)
            {
                const auto& ind = face_indices[i];
                assert(ind.size() == 3);
                mesh.face_vertices[i] = Vec3ui(ind[0], ind[1], ind[2]);
            }
        }
        catch (...)
        {
            mesh.clear();
            return false;
        }

        return true;
    }


    std::vector<std::array<double, 3>> PlyIO::getVertexNormals(happly::PLYData& data)
    {
        std::vector<std::array<double, 3> > result;

        const std::string& vertexElementName = "vertex";
        auto& elem = data.getElement(vertexElementName);
        if (!elem.hasProperty("nx"))
            return result;

        std::vector<double> nx = elem.getProperty<double>("nx");
        std::vector<double> ny = elem.getProperty<double>("ny");
        std::vector<double> nz = elem.getProperty<double>("nz");

        result.resize(nx.size());
        for (size_t i = 0; i < result.size(); i++)
        {
          result[i][0] = nx[i];
          result[i][1] = ny[i];
          result[i][2] = nz[i];
        }

        return result;
    }


} // namespace menderer
