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

#include <menderer/scene.h>

#include <iostream>

#include <menderer/ogl/render_context.h>


namespace menderer
{

    Scene::Scene(const Camera& camera, const ogl::MeshRenderer::Config &renderer_cfg) :
        camera_(camera),
        tex_color_(),
        tex_depth_(),
        fb_(),
        mesh_renderer_(renderer_cfg)
    {
        // set up frame buffer and textures
        tex_depth_.createDepth(camera_.width(), camera_.height());
        fb_.attach(tex_depth_);
        tex_color_.createBGR(ogl::Texture::UByte, camera_.width(), camera_.height());
        fb_.attach(tex_color_);
    }


    Scene::~Scene()
    {
        fb_.clear();
    }


    bool Scene::upload(const Mesh& mesh)
    {
        // upload mesh to GPU
        mesh_renderer_.update(mesh);
        return true;
    }


    bool Scene::render(const Mat4& pose_world_to_view, cv::Mat& color_out, cv::Mat &depth_out)
    {
        // set up framebuffer rendering
        fb_.drawBuffers();

        // configure render context
        ogl::RenderContext render_ctx;
        render_ctx.setPinholeProjection(camera_.width(), camera_.height(), camera_.intrinsics());
        render_ctx.setModelViewMatrix(pose_world_to_view);
        render_ctx.setViewport(0, 0, camera_.width(), camera_.height());
        // apply render context
        render_ctx.apply();

        // render the mesh
        mesh_renderer_.draw();

        // download target textures
        tex_color_.download(color_out);
        tex_depth_.download(depth_out);

        // restore projection and model view matrices
        render_ctx.restore();

        // scale depth buffer values to metric units
        render_ctx.convertDepthBufferToMetric(depth_out);

        return true;
    }


} // namespace menderer
