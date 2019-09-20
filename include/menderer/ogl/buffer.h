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


namespace menderer
{
namespace ogl
{

    /**
     * @brief   Wrapper class for OpenGL buffers.
     * @author  Robert Maier
     */
    class Buffer
    {
    public:

        /**
         * @brief   Constructor for creating a buffer.
         * @param   target          GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
         */
        Buffer(GLenum target);

        /// Destructor.
        ~Buffer();

        /// Bind the buffer.
        void bind();

        /// Unbind the buffer.
        void unbind();

        /// Upload data (from std::vector) onto the buffer on the GPU.
        template<typename T>
        bool upload(const std::vector<T> &data, GLenum usage = GL_STATIC_DRAW);

        /// Clear the buffer.
        void clear();

        /// Returns the size (number of elements) in the buffer.
        size_t size() const;

        /// Returns the byte size of the buffer.
        size_t byteSize() const;

        /// Checks if the buffer is empty.
        bool empty() const;

    private:
        Buffer(const Buffer&);
        Buffer& operator=(const Buffer&);

        /// Upload data (from raw data pointer) onto the buffer on the GPU.
        bool upload(size_t byte_size, const void* data, GLenum usage);

        GLuint id_;
        GLenum target_;
        size_t size_;
        size_t size_bytes_;
    };

} // namespace ogl
} // namespace menderer
