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

#include <menderer/ogl/ogl.h>

#include <map>
#include <opencv2/core/core.hpp>
#include <menderer/mat.h>


namespace menderer
{
namespace ogl
{

    /**
     * @brief   Wrapper class for OpenGL textures.
     * @author  Robert Maier
     */
    class Texture
    {
    public:

        /**
         * @brief   Enum for supported texture data types.
         * @author  Robert Maier
         */
        enum Type
        {
            Byte = 0,
            UByte = 1,
            Short = 2,
            UShort = 3,
            Int = 4,
            Float = 5,
            Double = 6
        };

        /// Constructor for creating an empty OpenGL texture.
        Texture();

        /// Destructor.
        ~Texture();

        /// Create a texture on GPU from cv:Mat and upload data.
        bool createFrom(const cv::Mat &img);

        /// Create a three-channel BGR texture on GPU.
        bool createBGR(Type type, int width, int height);

        /// Create a one-channel depth texture on GPU.
        bool createDepth(int width, int height);

        /// Reset/clear the texture.
        void reset();

        /// Bind texture.
        void bind(int unit = 0);

        /// Unbind texture.
        void unbind();

        /// Upload an image from cv::Mat to texture on GPU.
        bool upload(const cv::Mat &img);

        /// Download an image from the texture on GPU into cv::Mat.
        bool download(cv::Mat &img);

        /// Returns the texture id.
        unsigned int id() const;

        /// Returns the width of the texture.
        int width() const ;

        /// Returns the height of the texture.
        int height() const;

        /// Checks if texture is empty.
        bool empty() const;

        /// Checks if texture format is compatible with the given parameter.
        bool isFormatCompatible(const cv::Mat &img) const;

        /// Checks if texture format is used for storing depth.
        bool isDepth() const;

    private:
        Texture(const Texture&);
        Texture& operator=(const Texture&);

        /// Generates a texture and sets format and parameters.
        bool init(Type type, int width, int height, int internalFormat, GLenum imageFormat);

        /// Generates a texture and sets format and parameters from cv::Mat type.
        bool init(const cv::Mat &img);

        /// Upload data from raw pointer onto texture on GPU.
        bool upload(void* data);

        /// Creates an output cv::Mat that is compatible with the texture format.
        void createFromCurrent(cv::Mat &img) const;

        /// Retrieves texture format from cv::Mat.
        void getFormatFromOpenCV(const cv::Mat &img, int &internalFormat, GLenum &imageFormat) const;
        /// Retrieves data type from cv::Mat type.
        static Type cvtTypeFromOpenCV(int type);

        /// Converts texture data type to OpenGL data type.
        static GLenum convertTypeToOGL(Type type);
        /// Converts OpenGL data type to texture data type.
        static Type convertTypeFromOGL(GLenum type);

        unsigned int id_;
        int unit_;
        int internal_format_;
        GLenum image_format_;
        GLenum image_type_;
        int width_;
        int height_;
        Type type_;
    };

} // namespace ogl
} // namespace menderer
