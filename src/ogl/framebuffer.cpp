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

#include <menderer/ogl/framebuffer.h>

#include <iostream>

#include <menderer/ogl/texture.h>


namespace menderer
{
namespace ogl
{

    Framebuffer::Framebuffer() :
        id_(0),
        num_color_attachments_(0)
    {
        // generate framebuffer
        glGenFramebuffers(1, &id_);
    }


    Framebuffer::~Framebuffer()
    {
        if (id_)
            glDeleteFramebuffers(1, &id_);
    }


    void Framebuffer::bind()
    {
        if (id_)
            glBindFramebuffer(GL_FRAMEBUFFER, id_);
    }


    void Framebuffer::unbind()
    {
        if (id_)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    void Framebuffer::attach(const Texture &tex)
    {
        if (tex.empty())
            return;

        unsigned int attachment_id;
        if (tex.isDepth())
        {
            // depth attachment for depth buffer
            attachment_id = GL_DEPTH_ATTACHMENT;
        }
        else
        {
            // color attachment
            attachment_id = GL_COLOR_ATTACHMENT0 + num_color_attachments_;
            ++num_color_attachments_;
        }
        draw_buffers_.push_back(attachment_id);

        bind();

        // set texture as color/depth attachments
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_id, GL_TEXTURE_2D, tex.id(), 0);
    }


    bool Framebuffer::drawBuffers()
    {
        // set draw buffers list
        glDrawBuffers(static_cast<int>(draw_buffers_.size()), &draw_buffers_[0]);

        // check if framebuffer is ok
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return false;

        return true;
    }


    void Framebuffer::clear()
    {
        num_color_attachments_ = 0;

        // clear draw buffers
        GLenum buf = GL_NONE;
        glDrawBuffers(1, &buf);

        // detach attached textures
        for (size_t i = 0; i < draw_buffers_.size(); ++i)
            glFramebufferTexture(GL_FRAMEBUFFER, draw_buffers_[i], 0, 0);
        // clear attachments
        draw_buffers_.clear();

        // unbind framebuffer
        unbind();
    }

} // namespace ogl
} // namespace menderer
