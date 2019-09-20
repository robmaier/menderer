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

#include <menderer/ogl/buffer.h>

#include <iostream>


namespace menderer
{
namespace ogl
{

    Buffer::Buffer(GLenum target) :
        id_(0),
        target_(target),
        size_bytes_(0)
    {
    }


    Buffer::~Buffer()
    {
        if (id_)
            glDeleteBuffers(1, &id_);
    }


    void Buffer::bind()
    {
        if (!id_)
            return;
        glBindBuffer(target_, id_);
    }


    void Buffer::unbind()
    {
        if (!id_)
            return;
        glBindBuffer(target_, 0);
    }


    template<typename T>
    bool Buffer::upload(const std::vector<T> &data, GLenum usage)
    {
        // compute and store sizes
        size_ = data.size();
        size_bytes_ = sizeof(T) * data.size();
        return upload(size_bytes_, &(data[0]), usage);
    }

    // template method instantiations
    template bool Buffer::upload<Vec3b>(const std::vector<Vec3b> &data, GLenum usage);
    template bool Buffer::upload<Vec3f>(const std::vector<Vec3f> &data, GLenum usage);
    template bool Buffer::upload<Vec3i>(const std::vector<Vec3i> &data, GLenum usage);
    template bool Buffer::upload<Vec3ui>(const std::vector<Vec3ui> &data, GLenum usage);
    template bool Buffer::upload<Vec3>(const std::vector<Vec3> &data, GLenum usage);


    bool Buffer::upload(size_t size_bytes, const void* data, GLenum usage)
    {
        // generate buffers
        if (!id_)
            glGenBuffers(1, &id_);

        // upload data
        glBindBuffer(target_, id_);
        glBufferData(target_, static_cast<GLsizeiptr>(size_bytes), data, usage);

        return true;
    }


    void Buffer::clear()
    {
        std::vector<Vec3> data;
        upload(data);
    }


    size_t Buffer::size() const
    {
        return size_;
    }


    size_t Buffer::byteSize() const
    {
        return size_bytes_;
    }


    bool Buffer::empty() const
    {
        return size_bytes_ == 0;
    }


} // namespace ogl
} // namespace menderer
