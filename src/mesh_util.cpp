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

#include <menderer/mesh_util.h>

#include <iostream>
#include <map>
#include <tuple>


namespace menderer
{

    void MeshUtil::computeFaceNormals(const Mesh &mesh, std::vector<Vec3> &face_normals)
    {
        const std::vector<Vec3> &verts = mesh.vertices;
        const auto &face_indices = mesh.face_vertices;

        size_t num_faces = face_indices.size();
        face_normals.resize(num_faces);
        for (size_t i = 0; i < num_faces; ++i)
        {
            // collect vertex locations for current triangle
            unsigned int vert_idx0 = face_indices[i][0];
            unsigned int vert_idx1 = face_indices[i][1];
            unsigned int vert_idx2 = face_indices[i][2];
            Vec3 v0 = verts[vert_idx0];
            Vec3 v1 = verts[vert_idx1];
            Vec3 v2 = verts[vert_idx2];

            // compute normal from vertex locations usnig cross product
            Vec3 normal = (v1 - v0).cross(v2 - v0);
            normal.normalize();
            face_normals[i] = normal;
        }
    }


    void MeshUtil::computeVertexNormals(Mesh &mesh)
    {
        // introduce shorthands and clear vertex normals
        const std::vector<Vec3> &verts = mesh.vertices;
        std::vector<Vec3> &normals = mesh.normals;
        const auto& face_indices = mesh.face_vertices;
        size_t num_verts = verts.size();
        size_t num_faces = face_indices.size();
        normals.clear();

        // compute face normals
        std::vector<Vec3> face_normals;
        computeFaceNormals(mesh, face_normals);

        // compute vertex normals based on face normals

        // first collect faces for each vertex
        std::vector<std::vector<unsigned int> > vertex_faces;
        vertex_faces.resize(num_verts, std::vector<unsigned int>());
        for (unsigned int i = 0; i < num_faces; ++i)
        {
            for (int t = 0; t < 3; ++t)
            {
                unsigned int vIdx = face_indices[i][t];
                vertex_faces[vIdx].push_back(i);
            }
        }

        // compute vertex normals by averaging its face normals
        normals.resize(num_verts);
        for (size_t i = 0; i < num_verts; ++i)
        {
            Vec3 normal(0.0, 0.0, 0.0);
            if (vertex_faces[i].empty())
            {
                //std::cerr << "Vertex " << i << ": no faces found!" << std::endl;
            }
            else
            {
                for (size_t j = 0; j < vertex_faces[i].size(); ++j)
                {
                    unsigned int face_idx = vertex_faces[i][j];
                    normal += face_normals[face_idx];
                }
                normal.normalize();
            }
            normals[i] = normal;
        }

        // clear face normals
        face_normals.clear();
    }


    void MeshUtil::removeDegenerateFaces(Mesh &mesh)
    {
        // remove degenerate faces
        const std::vector<Vec3> &verts = mesh.vertices;
        auto &faces = mesh.face_vertices;
        std::vector<Vec3ui> faces_new;
        int num_degenerate_faces = 0;
        for (size_t i = 0; i < faces.size(); ++i)
        {
            unsigned int vert_idx0 = static_cast<unsigned int>(faces[i][0]);
            unsigned int vert_idx1 = static_cast<unsigned int>(faces[i][1]);
            unsigned int vert_idx2 = static_cast<unsigned int>(faces[i][2]);
            // check if two vertices have the same indices
            if ((vert_idx0 == vert_idx1) || (vert_idx0 == vert_idx2) || (vert_idx1 == vert_idx2))
            {
                ++num_degenerate_faces;
                continue;
            }

            // check if triangle area is zero
            if (computeFaceArea(verts[vert_idx0], verts[vert_idx1], verts[vert_idx2]) == 0.0)
            {
                ++num_degenerate_faces;
                continue;
            }

            Vec3ui idx(vert_idx0, vert_idx1, vert_idx2);
            faces_new.push_back(idx);
        }
        if (num_degenerate_faces > 0)
        {
            mesh.face_vertices = faces_new;
        }
    }


    double MeshUtil::computeFaceArea(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2)
    {
        double area = ((v2-v0).cross(v2-v1)).norm();
        if (std::isnan(area) || area == std::numeric_limits<double>::infinity())
            area = 0.0;
        return area;
    }


    void MeshUtil::compressVertices(Mesh &mesh)
    {
        // unused vertices are removed implicitely
        std::vector<Vec3> &verts = mesh.vertices;
        std::vector<Vec3> &normals = mesh.normals;
        std::vector<Vec3b> &colors = mesh.colors;
        auto& face_indices = mesh.face_vertices;
        size_t num_faces = face_indices.size();
        bool has_colors = !colors.empty();
        bool has_normals = !normals.empty();

        typedef std::tuple<double, double, double> vec3;
        std::vector<unsigned int> vert_indices;
        vert_indices.resize(verts.size());
        std::map<vec3, unsigned int> compressed_vert_indices;

        // unify vertices
        std::vector<Vec3> verts_new;
        std::vector<Vec3b> colors_new;
        std::vector<Vec3> normals_new;
        for (size_t i = 0; i < num_faces; i++)
        {
            for (int t = 0; t < 3; ++t)
            {
                // create tuple representation for current vertex
                unsigned int vIdx = static_cast<unsigned int>(face_indices[i][t]);
                Vec3 vIn = verts[vIdx];
                vec3 v = std::make_tuple(vIn[0], vIn[1], vIn[2]);

                // lookup index for this vertex
                if (compressed_vert_indices.find(v) == compressed_vert_indices.end())
                {
                    // vertex does not exist yet -> store it
                    compressed_vert_indices[v] = static_cast<unsigned int>(verts_new.size());

                    verts_new.push_back(vIn);
                    if (has_colors)
                        colors_new.push_back(colors[vIdx]);
                    if (has_normals)
                        normals_new.push_back(normals[vIdx]);
                }
                vert_indices[vIdx] = compressed_vert_indices[v];
            }
        }

        // update vertices with new unified vertices
        mesh.vertices = verts_new;
        mesh.normals = normals_new;
        mesh.colors = colors_new;

        // update face indices
        for (size_t i = 0; i < num_faces; ++i)
        {
            for (int t = 0; t < 3; ++t)
            {
                // create tuple representation for current vertex
                unsigned int vIdx = face_indices[i][t];
                face_indices[i][t] = vert_indices[vIdx];
            }
        }

        // remove degenerate faces
        removeDegenerateFaces(mesh);
    }


} // namespace menderer
