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
#include <menderer/ogl/ogl.h>

#include <vector>
#include <opencv2/core/core.hpp>

#include <menderer/camera.h>
#include <menderer/mesh.h>
#include <menderer/ogl/framebuffer.h>
#include <menderer/ogl/mesh_renderer.h>
#include <menderer/ogl/texture.h>


namespace menderer
{

    /**
     * @brief   Scene for OpenGL rendering.
     * @author  Robert Maier
     */
    class Scene
    {
    public:

        /**
         * @brief   Constructor for creating a scene for rendering.
         * @param   camera          Pinhole camera model.
         * @param   renderer_cfg    Mesh renderer configuration.
         */
        Scene(const Camera& camera, const ogl::MeshRenderer::Config &renderer_cfg);

        /// Destructor.
        ~Scene();

        /// Upload a mesh on the GPU.
        bool upload(const Mesh& mesh);

        /**
         * @brief   Renders the uploaded mesh into a synthetic color image and depth image
         *          from a specified pose.
         * @param   pose_world_to_cam   Target pose for rendering.
         * @param   color_out   Rendered color image.
         * @param   depth_out   Rendered depth map (from depth buffer).
         */
        bool render(const Mat4& pose_world_to_cam, cv::Mat& color_out, cv::Mat &depth_out);

    private:
        Camera camera_;
        ogl::Texture tex_color_;
        ogl::Texture tex_depth_;
        ogl::Framebuffer fb_;
        ogl::MeshRenderer mesh_renderer_;
    };

} // namespace menderer
