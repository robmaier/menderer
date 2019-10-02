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


    bool PlyIO::save(const std::string &filename, const Mesh &mesh, bool format_binary)
    {
        if (filename.empty())
            return false;

        try
        {
            // output containers
            std::vector<std::array<double, 3> > mesh_verts_out;
            std::vector<std::array<unsigned char, 3> > mesh_colors_out;
            std::vector<std::vector<size_t> > mesh_faces_out;

            // fill output containers from mesh

            // vertex positions
            mesh_verts_out.resize(mesh.vertices.size());
            for (size_t i = 0; i < mesh.vertices.size(); ++i)
            {
                const auto& v = mesh.vertices[i];
                std::array<double, 3> v_out;
                v_out[0] = v[0];
                v_out[1] = v[1];
                v_out[2] = v[2];
                mesh_verts_out[i] = v_out;
            }

            // vertex colors
            mesh_colors_out.resize(mesh.colors.size());
            for (size_t i = 0; i < mesh.colors.size(); ++i)
            {
                const auto& c = mesh.colors[i];
                std::array<unsigned char, 3> c_out;
                c_out[0] = c[0];
                c_out[1] = c[1];
                c_out[2] = c[2];
                mesh_colors_out[i] = c_out;
            }

            // faces
            mesh_faces_out.resize(mesh.face_vertices.size());
            for (size_t i = 0; i < mesh.face_vertices.size(); ++i)
            {
                const auto& ind = mesh.face_vertices[i];
                assert(ind.size() == 3);
                std::vector<size_t> ind_out(3);
                ind_out[0] = static_cast<size_t>(ind[0]);
                ind_out[1] = static_cast<size_t>(ind[1]);
                ind_out[2] = static_cast<size_t>(ind[2]);
                mesh_faces_out[i] = ind_out;
            }

            // save ply file using happly
            happly::PLYData ply_out;
            ply_out.addVertexPositions(mesh_verts_out);
            ply_out.addVertexColors(mesh_colors_out);
            ply_out.addFaceIndices(mesh_faces_out);
            happly::DataFormat fmt = format_binary ? happly::DataFormat::Binary : happly::DataFormat::ASCII;
            ply_out.write(filename, fmt);
        }
        catch (...)
        {
            return false;
        }

        return true;
    }

} // namespace menderer
