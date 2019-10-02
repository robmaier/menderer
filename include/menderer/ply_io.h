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

#include <string>
#include <happly/happly.h>
#include <menderer/mesh.h>


namespace menderer
{

    /** @brief	PLY input/output
    *	@author	Robert Maier
    */
    class PlyIO
    {
    public:

        /// Load ply file using happly library.
        static bool load(const std::string &filename, Mesh &mesh);

        /// Save mesh to ply file using happly library.
        static bool save(const std::string &filename, const Mesh &mesh, bool format_binary = true);

    private:
        /// Internal helper function for retrieving vertex normals from ply file.
        static std::vector<std::array<double, 3>> getVertexNormals(happly::PLYData& data);
    };

} // namespace menderer
