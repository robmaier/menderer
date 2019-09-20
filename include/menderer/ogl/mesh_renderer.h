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

#include <vector>
#include <menderer/mat.h>
#include <menderer/mesh.h>
#include <menderer/ogl/buffer.h>
#include <menderer/ogl/program.h>


namespace menderer
{
namespace ogl
{

    /**
     * @brief   OpenGL mesh renderer.
     * @author  Robert Maier
     */
    class MeshRenderer
    {
    public:

        /**
         * @brief   Mesh renderer configuration struct
         * @author  Robert Maier
         */
        struct Config
        {
        public:

            std::string shader = "normals_phong";
            Vec4f color = Vec4f(1.0f, 0.9f, 0.75f,1.0f);     //Vec4f(0.8f, 0.8f, 0.8f, 1.0f);
            Vec4f background = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
            bool lighting = false;
            bool colored = true;
            bool smooth = true;
            bool cull_backfaces = false;

            /// Print out renderer configuration.
            void print() const;
        };


        /**
         * @brief   Constructor for creating the mesh renderer.
         * @param   cfg     Mesh renderer configuration.
         */
        MeshRenderer(const Config& cfg);

        /// Destructor.
        ~MeshRenderer();

        /// Configure the mesh renderer.
        void configure(const Config& cfg);

        /// Returns the mesh renderer config.
        const Config& config() const;

        /// Upload the mesh onto the buffers on the GPU.
        void update(const Mesh &mesh);

        /// Render the mesh.
        void draw();

        /// Create shader for rendering from shader name.
        void createShader(const std::string &shader_name = "");

    private:
        /// Set up the lighting for rendering.
        void setupLighting();

        /// Set up the material for rendering.
        void setupMaterial();

        Config cfg_;

        size_t num_triangles_;
        Buffer buf_verts_;
        Buffer buf_colors_;
        Buffer buf_normals_;
        Buffer buf_indices_;
        Program program_;
    };

} // namespace ogl
} // namespace menderer
