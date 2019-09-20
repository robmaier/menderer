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
#include <menderer/mat.h>


namespace menderer
{
namespace ogl
{

    class Texture;


    /**
     * @brief   Wrapper class for OpenGL shader programs.
     * @author  Robert Maier
     */
    class Program
    {
    public:

        /**
         * @brief   Enum for different shader types.
         * @author  Robert Maier
         */
        enum ShaderType
        {
            VertexShader = 0,
            FragmentShader = 1,
            GeometryShader = 2
        };


        /// Constructor for creating an empty OpenGL shader program.
        Program();

        /// Destructor.
        ~Program();

        /// Create a program from files for vertex/fragment/geometry shader.
        bool create(const std::string &vert_shader = "", const std::string &frag_shader = "", const std::string &geom_shader = "");

        /// Checks if program is valid and shaders are set up correctly.
        bool valid() const;

        /// Reset the program.
        void reset();

        /// Add texture as uniform.
        void add(const std::string &name, Texture* texture);
        /// Add int scalar as uniform.
        void add(const std::string &name, int val);
        /// Add float scalar as uniform.
        void add(const std::string &name, float val);
        /// Add double scalar as uniform.
        void add(const std::string &name, double val);
        /// Add 2d double vector as uniform.
        void add(const std::string &name, const Vec2 &vec);
        /// Add 2d float vector as uniform.
        void add(const std::string &name, const Vec2f &vec);
        /// Add 2d int vector as uniform.
        void add(const std::string &name, const Vec2i &vec);
        /// Add 3d double vector as uniform.
        void add(const std::string &name, const Vec3 &vec);
        /// Add 3d float vector as uniform.
        void add(const std::string &name, const Vec3f &vec);
        /// Add 3d int vector as uniform.
        void add(const std::string &name, const Vec3i &vec);
        /// Add 4d double vector as uniform.
        void add(const std::string &name, const Vec4 &vec);
        /// Add 4d float vector as uniform.
        void add(const std::string &name, const Vec4f &vec);
        /// Add 4d int vector as uniform.
        void add(const std::string &name, const Vec4i &vec);
        /// Add 2x2 double matrix as uniform.
        void add(const std::string &name, const Mat2 &mat);
        /// Add 2x2 float matrix as uniform.
        void add(const std::string &name, const Mat2f &mat);
        /// Add 3x3 double matrix as uniform.
        void add(const std::string &name, const Mat3 &mat);
        /// Add 3x3 float matrix as uniform.
        void add(const std::string &name, const Mat3f &mat);
        /// Add 4x4 double matrix as uniform.
        void add(const std::string &name, const Mat4 &mat);
        /// Add 4x4 float matrix as uniform.
        void add(const std::string &name, const Mat4f &mat);

        /// Enable shader for rendering.
        void enable();
        /// Disable shader for rendering and unbind textures.
        void disable();

    protected:
        Program(const Program&);
        Program& operator=(const Program&);

        /// Add a specific shader.
        bool addShader(ShaderType type, const std::string &name);
        /// Load a shader from file.
        std::string loadShader(const std::string &filename);
        /// Create shader.
        bool createShader(unsigned int &shader_id, ShaderType type, const std::string &name);
        /// Delete shader.
        void deleteShader(unsigned int &shader_id);
        /// Check shader compilation status.
        bool checkShaderCompiled(GLuint id, const std::string& message) const;

        /// Get uniform location.
        int uniformLoc(const std::string &name) const;

        /// Compile program.
        bool compile();
        /// Check program linking status.
        bool checkProgramLinked(GLuint program_id, const std::string& message) const;

        unsigned int program_id_;
        unsigned int fragment_shader_id_;
        unsigned int vertex_shader_id_;
        unsigned int geometry_shader_id_;
        bool valid_;
        std::vector<Texture*> textures_;
        std::string shader_folder_;
    };

} // namespace ogl
} // namespace menderer
