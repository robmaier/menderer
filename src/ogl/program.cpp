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

#include <menderer/ogl/program.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <menderer/ogl/texture.h>

#define STR1(x)  #x
#define STR(x)  STR1(x)


namespace menderer
{
namespace ogl
{

    Program::Program() :
        program_id_(0),
        fragment_shader_id_(0),
        vertex_shader_id_(0),
        geometry_shader_id_(0),
        valid_(false),
        shader_folder_(std::string(STR(APP_SOURCE_DIR)) + "/src/ogl/shaders/")
    {
    }


    Program::~Program()
    {
        reset();
    }


    bool Program::create(const std::string &vert_shader, const std::string &frag_shader, const std::string &geom_shader)
    {
        // create program
        if (!program_id_)
            program_id_ = glCreateProgram();

        // create OpenGL shader program
        if (!vert_shader.empty())
        {
            if (!addShader(Program::VertexShader, vert_shader))
                std::cerr << "vertex shader could not be created!" << std::endl;
        }
        if (!frag_shader.empty())
        {
            if (!addShader(Program::FragmentShader, frag_shader))
                std::cerr << "fragment shader could not be created!" << std::endl;
        }
        if (!geom_shader.empty())
        {
            if (!addShader(Program::FragmentShader, geom_shader))
                std::cerr << "geometry shader could not be created!" << std::endl;
        }

        if (!compile())
        {
            reset();
            return false;
        }

        valid_ = true;
        return true;
    }


    bool Program::valid() const
    {
        return valid_;
    }


    void Program::reset()
    {
        // detach and delete shaders
        if (fragment_shader_id_)
        {
            deleteShader(fragment_shader_id_);
            fragment_shader_id_ = 0;
        }
        if (vertex_shader_id_)
        {
            deleteShader(vertex_shader_id_);
            vertex_shader_id_ = 0;
        }
        if (geometry_shader_id_)
        {
            deleteShader(geometry_shader_id_);
            geometry_shader_id_ = 0;
        }

        // delete program
        if (program_id_)
        {
            glDeleteProgram(program_id_);
            program_id_ = 0;
        }

        valid_ = false;
    }


    bool Program::compile()
    {
        if (valid_)
            return true;

        // attach shaders
        if (fragment_shader_id_)
        {
            glAttachShader(program_id_, fragment_shader_id_);
            fragment_shader_id_ = 0;
        }
        if (vertex_shader_id_)
            glAttachShader(program_id_, vertex_shader_id_);
        if (geometry_shader_id_)
            glAttachShader(program_id_, geometry_shader_id_);

        // link program
        glLinkProgram(program_id_);
        bool ok = checkProgramLinked(program_id_, "program linking failed!");

        return ok;
    }


    bool Program::addShader(ShaderType type, const std::string &name)
    {
        bool ok;
        if (type == FragmentShader)
            ok = createShader(fragment_shader_id_, type, name);
        else if (type == VertexShader)
            ok = createShader(vertex_shader_id_, type, name);
        else
            ok = createShader(geometry_shader_id_, type, name);
        return ok;
    }


    void Program::add(const std::string &name, Texture* texture)
    {
        int unit = static_cast<int>(textures_.size());
        //std::cout << "texture " << name << ": " << unit << std::endl;
        textures_.push_back(texture);

        texture->bind(unit);
        glUniform1i(uniformLoc(name), unit);
    }


    void Program::add(const std::string &name, int val)
    {
        glUniform1i(uniformLoc(name), val);
    }


    void Program::add(const std::string &name, float val)
    {
        glUniform1f(uniformLoc(name), val);
    }


    void Program::add(const std::string &name, double val)
    {
        add(name, static_cast<float>(val));
    }


    void Program::add(const std::string &name, const Vec2 &vec)
    {
        Vec2f v = vec.cast<float>();
        add(name, v);
    }


    void Program::add(const std::string &name, const Vec2f &vec)
    {
        glUniform2fv(uniformLoc(name), 1, vec.data());
    }


    void Program::add(const std::string &name, const Vec2i &vec)
    {
        glUniform2iv(uniformLoc(name), 1, vec.data());
    }


    void Program::add(const std::string &name, const Vec3 &vec)
    {
        Vec3f v = vec.cast<float>();
        add(name, v);
    }


    void Program::add(const std::string &name, const Vec3f &vec)
    {
        glUniform3fv(uniformLoc(name), 1, vec.data());
    }


    void Program::add(const std::string &name, const Vec3i &vec)
    {
        glUniform3iv(uniformLoc(name), 1, vec.data());
    }


    void Program::add(const std::string &name, const Vec4 &vec)
    {
        Vec4f v = vec.cast<float>();
        add(name, v);
    }


    void Program::add(const std::string &name, const Vec4f &vec)
    {
        glUniform4fv(uniformLoc(name), 1, vec.data());
    }


    void Program::add(const std::string &name, const Vec4i &vec)
    {
        glUniform4iv(uniformLoc(name), 1, vec.data());
    }


    void Program::add(const std::string &name, const Mat2 &mat)
    {
        Mat2f m = mat.cast<float>();
        add(name, m);
    }


    void Program::add(const std::string &name, const Mat2f &mat)
    {
        glUniformMatrix2fv(uniformLoc(name), 1, 0, mat.data());
    }


    void Program::add(const std::string &name, const Mat3 &mat)
    {
        Mat3f m = mat.cast<float>();
        add(name, m);
    }


    void Program::add(const std::string &name, const Mat3f &mat)
    {
        glUniformMatrix3fv(uniformLoc(name), 1, 0, mat.data());
    }


    void Program::add(const std::string &name, const Mat4 &mat)
    {
        Mat4f m = mat.cast<float>();
        add(name, m);
    }


    void Program::add(const std::string &name, const Mat4f &mat)
    {
        glUniformMatrix4fv(uniformLoc(name), 1, 0, mat.data());
    }


    int Program::uniformLoc(const std::string &name) const
    {
        return glGetUniformLocation(program_id_, name.c_str());
    }


    void Program::enable()
    {
        // use program
        glUseProgram(program_id_);
    }

    \
    void Program::disable()
    {
        // unuse program
        glUseProgram(0);

        // unbind textures
        for (size_t i = 0; i < textures_.size(); ++i)
        {
            Texture* texture = textures_[i];
            texture->unbind();
        }
        textures_.clear();
    }


    bool Program::createShader(unsigned int &shader_id, ShaderType type, const std::string &name)
    {
        GLenum shader_type;
        switch (type)
        {
        case FragmentShader:
            shader_type = GL_FRAGMENT_SHADER; break;
        case VertexShader:
            shader_type = GL_VERTEX_SHADER; break;
        default:
            shader_type = GL_GEOMETRY_SHADER; break;
        }

        // load shader code from file
        std::string shader_code = loadShader(name);
        if (shader_code.empty())
        {
            shader_id = 0;
            return false;
        }

        // create shader
        shader_id = glCreateShader(shader_type);

        // set shader source
        const char *code = shader_code.c_str();
        GLint length = static_cast<GLint>(shader_code.size());
        glShaderSource(shader_id, 1, &code, &length);

        // compile shader
        glCompileShader(shader_id);
        bool ok = checkShaderCompiled(shader_id, "Shader compilation failed!");

        return ok;
    }


    std::string Program::loadShader(const std::string &name)
    {
        std::string code = "";
        std::ifstream file(shader_folder_ + name);
        if (!file.is_open())
            return code;

        std::stringstream ss;
        std::string line;
        while(std::getline(file, line))
            ss << line << std::endl;
        file.close();
        code = ss.str();

        return code;
    }


    void Program::deleteShader(unsigned int &shader_id)
    {
        glDetachShader(program_id_, shader_id);
        glDeleteShader(shader_id);
    }


    bool Program::checkShaderCompiled(GLuint shader_id, const std::string& message) const
    {
        GLint status;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);

        if (status != GL_TRUE)
        {
            char buffer[4096];
            GLsizei length;
            glGetShaderInfoLog(shader_id, 4096, &length, buffer);
            std::cerr << message << std::endl;
            std::cerr << std::string(buffer, static_cast<size_t>(length)) << std::endl;
            //throw std::exception();
            return false;
        }
        return true;
    }

    bool Program::checkProgramLinked(GLuint program_id, const std::string& message) const
    {
        GLint status;
        glGetProgramiv(program_id, GL_LINK_STATUS, &status);

        if (status != GL_TRUE)
        {
            char buffer[4096];
            GLsizei length;
            glGetProgramInfoLog(program_id, 4096, &length, buffer);
            std::cerr << message << std::endl << std::string(buffer, static_cast<size_t>(length)) << std::endl;
            //throw std::exception();
            return false;
        }
        return true;
    }


} // namespace ogl
} // namespace menderer
