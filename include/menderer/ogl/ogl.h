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

// define GL_GLEXT_PROTOTYPES for using glGenBuffers etc
#define GL_GLEXT_PROTOTYPES
// include GLFW (which also includes all required OpenGL headers)
#include <GLFW/glfw3.h>

#include <iostream>


namespace menderer
{
namespace ogl
{

    /// Creates OpenGL context using GLFW
    static inline bool createContext()
    {
        // create OpenGL context

        // init GLFW
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
            return false;
        }
        // create GLFW offscreen context
        glfwWindowHint(GLFW_VISIBLE, false);
        GLFWwindow* ogl_context = glfwCreateWindow(1, 1, "", nullptr, nullptr);
        if (!ogl_context)
        {
            std::cerr << "Failed to create GLFW context!" << std::endl;
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(ogl_context);

        return true;
    }


    /// Destroys OpenGL context that was created using GLFW
    static inline void destroyContext()
    {
        // destroy OpenGl context
        glfwTerminate();
    }

} // namespace ogl
} // namespace menderer
