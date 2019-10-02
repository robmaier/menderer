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
#include <menderer/mesh.h>


namespace menderer
{

    /**
     * @brief   Basic mesh processing functionality.
     * @author  Robert Maier
     */
    class MeshUtil
    {
    public:

        /// Compute the per-vertex normals of a mesh.
        static void computeVertexNormals(Mesh &mesh);

        /// Compress mesh vertices by removing redundant/duplicate vertices.
        static void compressVertices(Mesh &mesh);

        /// Create mesh from RGB-D frame.
        static bool createFromRGBD(const cv::Mat &vertex_map, const cv::Mat &color,
                                   const Mat4 &pose_cam_to_world, Mesh &mesh);

    private:
        /// Compute the per-triangle face normals for all faces of a mesh.
        static void computeFaceNormals(const Mesh &mesh, std::vector<Vec3> &faceNormals);

        /// Remove degenerate triangles of a mesh.
        static void removeDegenerateFaces(Mesh &mesh);

        /// Compute the area of a single triangle.
        static double computeFaceArea(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2);

    };

} // namespace menderer
