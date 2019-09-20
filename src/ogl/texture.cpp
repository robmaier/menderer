#include <menderer/ogl/texture.h>

#include <iostream>
#include <fstream>
#include <sstream>


namespace menderer
{
namespace ogl
{

    Texture::Texture() :
        id_(0),
        unit_(-1),
        internal_format_(0),
        image_format_(0),
        image_type_(0),
        width_(0),
        height_(0),
        type_(UByte)
    {
    }


    Texture::~Texture()
    {
        reset();
    }


    bool Texture::createFrom(const cv::Mat &img)
    {
        bool ok = init(img);
        if (ok)
            ok = upload(static_cast<void*>(img.data));
        if (!ok)
            reset();
        return ok;
    }


    bool Texture::createBGR(Type type, int width, int height)
    {
        return init(type, width, height, GL_RGB, GL_BGR);
    }


    bool Texture::createDepth(int width, int height)
    {
        return init(Float, width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT);
    }


    bool Texture::init(const cv::Mat &img)
    {
        // determine image type
        Type t = Texture::cvtTypeFromOpenCV(img.type());
        // determine image format
        int internal_format;
        GLenum image_format;
        getFormatFromOpenCV(img, internal_format, image_format);

        // init
        return init(t, img.cols, img.rows, internal_format, image_format);
    }


    bool Texture::init(Type type, int width, int height, int internal_format, GLenum image_format)
    {
        if (!id_)
        {
            glGenTextures(1, &id_);
            if (!id_)
                return false;
        }

        // set image size and format
        type_ = type;
        width_ = width;
        height_ = height;
        image_type_ = convertTypeToOGL(type);
        internal_format_ = internal_format;
        image_format_ = image_format;

        // set texture interpolation and clamping
        bind();
        // set texture clamping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        // set texture interpolation
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        unbind();

        // upload
        if (!empty())
            upload(nullptr);

        return true;
    }


    void Texture::reset()
    {
        if (id_)
            glDeleteTextures(1, &id_);

        id_ = 0;
        unit_ = -1;
        internal_format_ = 0;
        image_format_ = 0;
        image_type_ = 0;
        width_ = 0;
        height_ = 0;
        type_ = UByte;
    }


    void Texture::bind(int unit)
    {
        if (!id_)
            return;
        unit_ = unit;
        glActiveTexture(GL_TEXTURE0 + unit_);
        glBindTexture(GL_TEXTURE_2D, id_);
    }


    void Texture::unbind()
    {
        if (!id_)
            return;
        glActiveTexture(GL_TEXTURE0 + unit_);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    bool Texture::upload(const cv::Mat &img)
    {
        if (!id_ || img.empty())
            return false;

        if (empty())
        {
            // init image if not initialized
            if (!init(img))
                return false;
        }
        else
        {
            // check if input image format is compatible
            if (!isFormatCompatible(img))
                return false;
        }

        // upload image to texture
        upload(static_cast<void*>(img.data));

        return true;
    }


    bool Texture::upload(void* data)
    {
        // set tight OpenGL row alignment state
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // upload data to texture
        bind();
        if (data)
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, image_format_, image_type_, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format_, width_, height_, 0, image_format_, image_type_, data);
        unbind();

        return true;
    }


    bool Texture::download(cv::Mat &img)
    {
        if (!id_ || empty())
            return false;

        // create output image if not created yet or if sizes don't match
        if (img.empty())
        {
            img.release();
            // create from internal format
            createFromCurrent(img);
        }

        // set tight OpenGL row alignment state
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        // download texture
        bind();
        glGetTexImage(GL_TEXTURE_2D, 0, image_format_, image_type_, img.data);
        unbind();

        return true;
    }


    void Texture::getFormatFromOpenCV(const cv::Mat &img, int &internal_format, GLenum &image_format) const
    {
        // determine input image format
        int num_channels = img.channels();
        if (num_channels == 1)
            image_format = GL_LUMINANCE;
        else if (num_channels == 2)
            image_format = GL_LUMINANCE_ALPHA;
        else if (num_channels == 3)
            image_format = GL_BGR;
        else if (num_channels == 4)
            image_format = GL_BGRA;
        else
            image_format = 0;

        // determine input image type
        Type type = Texture::cvtTypeFromOpenCV(img.type());

        // determine OpenGL internal texture format based on input image
        internal_format = GL_RGB;
        if (type == UByte)
        {
            if (num_channels == 1)
                internal_format = GL_LUMINANCE;
            else if (num_channels == 2)
                internal_format = GL_LUMINANCE_ALPHA;
            else if (num_channels == 3)
                internal_format = GL_RGB;
            else if (num_channels == 4)
                internal_format = GL_RGBA;
        }
        else if (type == UShort)
        {
            if (num_channels == 1)
                internal_format = GL_DEPTH_COMPONENT16;  //GL_INTENSITY16;
        }
        else if (type == Float)
        {
            if (num_channels == 1)
                internal_format = GL_R32F;   //GL_LUMINANCE32F_ARB //GL_INTENSITY32F_ARB;
            else if (num_channels == 2)
                internal_format = GL_LUMINANCE_ALPHA32F_ARB;
            else if (num_channels == 3)
                internal_format = GL_RGB32F;
            else if (num_channels == 4)
                internal_format = GL_RGBA32F;
        }
    }


    Texture::Type Texture::cvtTypeFromOpenCV(int type)
    {
        Type t;
        if (type == CV_8S)
            t = Byte;
        else if (type == CV_16U)
            t = UShort;
        else if (type == CV_16S)
            t = Short;
        else if (type == CV_32S)
            t = Int;
        else if (type == CV_32F)
            t = Float;
        else if (type == CV_64F)
            t = Double;
        else
            t = UByte;
        return t;
    }


    GLenum Texture::convertTypeToOGL(Type type)
    {
        GLenum t;
        if (type == Byte)
            t = GL_BYTE;
        else if (type == UShort)
            t = GL_UNSIGNED_SHORT;
        else if (type == Short)
            t = GL_SHORT;
        else if (type == Int)
            t = GL_INT;
        else if (type == Float)
            t = GL_FLOAT;
        else if (type == Double)
            t = GL_DOUBLE;
        else
            t = GL_UNSIGNED_BYTE;
        return t;
    }


    Texture::Type Texture::convertTypeFromOGL(GLenum type)
    {
        Type t;
        if (type == GL_BYTE)
            t = Byte;
        else if (type == GL_UNSIGNED_SHORT)
            t = UShort;
        else if (type == GL_SHORT)
            t = Short;
        else if (type == GL_INT)
            t = Int;
        else if (type == GL_FLOAT)
            t = Float;
        else if (type == GL_DOUBLE)
            t = Double;
        else
            t = UByte;
        return t;
    }


    void Texture::createFromCurrent(cv::Mat &img) const
    {
        // determine OpenCV image type from internal format
        int num_channels = 0;
        if (image_format_ == GL_LUMINANCE || image_format_ == GL_RED || image_format_ == GL_INTENSITY)
            num_channels = 1;
        else if (image_format_ == GL_LUMINANCE_ALPHA)
            num_channels = 2;
        else if (image_format_ == GL_BGR || image_format_ == GL_RGB)
            num_channels = 3;
        else if (image_format_ == GL_BGRA || image_format_ == GL_RGBA)
            num_channels = 4;

        int t = 0;
        if (type_ == UByte)
        {
            if (num_channels == 1)
                t = CV_8UC1;
            else if (num_channels == 2)
                t = CV_8UC2;
            else if (num_channels == 3)
                t = CV_8UC3;
            else if (num_channels == 4)
                t = CV_8UC4;
        }
        else if (type_ == UShort)
        {
            if (num_channels == 1)
                t = CV_16UC1;
            else if (num_channels == 2)
                t = CV_16UC2;
            else if (num_channels == 3)
                t = CV_16UC3;
            else if (num_channels == 4)
                t = CV_16UC4;
        }
        else if (type_ == Float)
        {
            if (num_channels == 1)
                t = CV_32FC1;
            else if (num_channels == 2)
                t = CV_32FC2;
            else if (num_channels == 3)
                t = CV_32FC3;
            else if (num_channels == 4)
                t = CV_32FC4;
        }

        // special cases
        if (t == 0 && image_format_ == GL_DEPTH_COMPONENT)
            t = CV_32FC1;

        if (t != 0)
        {
            // create OpenCV image
            img.create(height_, width_, t);
        }
    }


    unsigned int Texture::id() const
    {
        return id_;
    }

    int Texture::width() const
    {
        return width_;
    }


    int Texture::height() const
    {
        return height_;
    }


    bool Texture::empty() const
    {
        return (width_ * height_ == 0);
    }


    bool Texture::isFormatCompatible(const cv::Mat &img) const
    {
        // image type
        Type t = Texture::cvtTypeFromOpenCV(img.type());
        if (t != type_)
            return false;

        // image dimensions
        if (img.rows != height_ || img.cols != width_)
            return false;

        // check if input image format is right
        int internal_format;
        GLenum img_format;
        getFormatFromOpenCV(img, internal_format, img_format);
        if (internal_format != internal_format_ || img_format != image_format_)
            return false;

        return true;
    }


    bool Texture::isDepth() const
    {
        bool depth = (internal_format_ == GL_DEPTH_COMPONENT || internal_format_ == GL_DEPTH_COMPONENT16 ||
                      internal_format_ == GL_DEPTH_COMPONENT24 || internal_format_ == GL_DEPTH_COMPONENT32F);
        return depth;
    }

} // namespace ogl
} // namespace menderer
