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

#include <menderer/ogl/mesh_renderer.h>

#include <iostream>


namespace menderer
{
namespace ogl
{

    void MeshRenderer::Config::print() const
    {
        std::cout << "mesh renderer config: " << std::endl;
        std::cout << "   shader: " << shader << std::endl;
        std::cout << "   color: " << color.transpose() << std::endl;
        std::cout << "   background: " << background.transpose() << std::endl;
        std::cout << "   lighting: " << lighting << std::endl;
        std::cout << "   colored: " << colored << std::endl;
        std::cout << "   smooth: " << smooth << std::endl;
        //std::cout << "   cull_backfaces: " << cull_backfaces << std::endl;
    }


    MeshRenderer::MeshRenderer(const Config& cfg) :
        cfg_(cfg),
        num_triangles_(0),
        buf_verts_(GL_ARRAY_BUFFER),
        buf_colors_(GL_ARRAY_BUFFER),
        buf_normals_(GL_ARRAY_BUFFER),
        buf_indices_(GL_ELEMENT_ARRAY_BUFFER),
        program_()
    {
        configure(cfg);
    }


    MeshRenderer::~MeshRenderer()
    {
    }


    void MeshRenderer::configure(const Config& cfg)
    {
        cfg_ = cfg;
        createShader(cfg_.shader);
    }


    const MeshRenderer::Config& MeshRenderer::config() const
    {
        return cfg_;
    }


    void MeshRenderer::update(const Mesh& mesh)
    {
        // upload mesh to GPU
        buf_verts_.upload(mesh.vertices);
        buf_colors_.upload(mesh.colors);
        buf_normals_.upload(mesh.normals);
        buf_indices_.upload(mesh.face_vertices);
        num_triangles_ = mesh.face_vertices.size();
    }


    void MeshRenderer::draw()
    {
        if (buf_verts_.empty() || num_triangles_ == 0)
            return;

        // fill background
        glClearColor(cfg_.background[0], cfg_.background[1], cfg_.background[2], cfg_.background[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure depth test
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // set up lighting
        if (cfg_.lighting)
            setupLighting();

        // shade model
        glShadeModel(cfg_.smooth ? GL_SMOOTH : GL_FLAT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_MULTISAMPLE);

        // set up color and material
        glColor4fv(cfg_.color.data());
        setupMaterial();

        glEnableClientState(GL_VERTEX_ARRAY);
        buf_verts_.bind();
        glVertexPointer(3, GL_DOUBLE, 0, nullptr);

        if (!buf_normals_.empty())
        {
            glEnableClientState(GL_NORMAL_ARRAY);
            buf_normals_.bind();
            glNormalPointer(GL_DOUBLE, 0, nullptr);
        }

        // set up colors
        if (cfg_.colored && !buf_colors_.empty())
        {
            glEnableClientState(GL_COLOR_ARRAY);
            buf_colors_.bind();
            glColorPointer(3, GL_UNSIGNED_BYTE, 0, nullptr);
        }

        if (cfg_.cull_backfaces)
        {
            // backface culling
            glCullFace(GL_BACK);
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }

        // initialize shader
        if (program_.valid())
            program_.enable();

        // draw triangles using index buffer
        buf_indices_.bind();
        glDrawElements(GL_TRIANGLES, static_cast<GLint>(num_triangles_ * 3),
                       GL_UNSIGNED_INT, nullptr);

        // disable client states
        glDisableClientState(GL_VERTEX_ARRAY);
        if (!buf_normals_.empty())
            glDisableClientState(GL_NORMAL_ARRAY);
        if (cfg_.colored && !buf_colors_.empty())
            glDisableClientState(GL_COLOR_ARRAY);

        // disable shader
        if (program_.valid())
            program_.disable();

        glDisable(GL_MULTISAMPLE);
        glDisable(GL_BLEND);
        glPopAttrib();

        glPopMatrix();
    }


    void MeshRenderer::setupLighting()
    {
        glDisable(GL_TEXTURE_1D);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_3D);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_NORMALIZE);

        // light color
        Vec4f ambient(0.2f, 0.2f, 0.2f, 1.0f);
        Vec4f diffuse(0.7f, 0.7f, 0.7f, 1.0f);
        Vec4f specular(0.0f, 0.0f, 0.0f, 1.0f);
        if (program_.valid())
        {
            ambient = Vec4f(0.2f, 0.2f, 0.2f, 1.0f);
            diffuse = Vec4f(0.6f, 0.6f, 0.6f, 1.0f);
            specular = Vec4f(0.8f, 0.8f, 0.8f, 1.0f);
        }

        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient.data());
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse.data());
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular.data());

        // light position
        //float position[] = {0.0f, 0.0f, 1.0f, 1.0f};
        //glLightfv(GL_LIGHT0, GL_POSITION, position);
    }


    void MeshRenderer::setupMaterial()
    {
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

        Vec4f mat_color = cfg_.color;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_color.data());
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_color.data());
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_color.data());

        // shininess (between 0.0f and 128.0f, with 128.0f being less shiny)
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 96.0f);
    }


    void MeshRenderer::createShader(const std::string &shader_name)
    {
        if (program_.valid())
        {
            // delete shader if existing
            program_.reset();
        }

        if (shader_name.empty() || shader_name == "none")
            return;

        // create shader
        if (!shader_name.empty())
        {
            program_.create(shader_name + ".vs", shader_name + ".fs");
        }
    }

} // namespace ogl
} // namespace menderer
