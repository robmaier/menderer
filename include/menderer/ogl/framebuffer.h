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
#include <menderer/ogl/ogl.h>


namespace menderer
{
namespace ogl
{

    class Texture;


    /**
     * @brief   Wrapper class for OpenGL frame buffers.
     * @author  Robert Maier
     */
    class Framebuffer
    {
    public:

        /// Constructor for creating an OpenGL frame buffer.
        Framebuffer();

        /// Destructor.
        ~Framebuffer();

        /// Bind the frame buffer.
        void bind();

        /// Unbind the frame buffer.
        void unbind();

        /// Set draw buffers list.
        bool drawBuffers();

        /// Attach target output textures used for frame buffer drawing.
        void attach(const Texture& tex);

        /// Clear draw buffers and detach attached textures.
        void clear();

    private:
        Framebuffer(const Framebuffer&);
        Framebuffer& operator=(const Framebuffer&);

        GLuint id_;
        unsigned int num_color_attachments_;
        std::vector<GLenum> draw_buffers_;
    };

} // namespace ogl
} // namespace menderer
