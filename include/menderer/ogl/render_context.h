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
#include <opencv2/core/core.hpp>


namespace menderer
{
namespace ogl
{

    /**
     * @brief   Utility class for OpenGL projection and model-view matrices etc.
     * @author  Robert Maier
     */
    class RenderContext
    {
    public:

        /// Create render context with default parameters.
        RenderContext();

        /// Destructor.
        ~RenderContext();

        /// Converts pinhole camera parameters to OpenGL projection matrix.
        void setPinholeProjection(int width, int height, const Mat3 &K);
        /// Converts pinhole camera parameters to OpenGL projection matrix.
        void setPinholeProjection(int width, int height, double fx, double fy, double cx, double cy);
        /// Returns the OpenGL projection matrix.
        Mat4 projectionMatrix() const;

        /// Set the OpenGL model-view matrix.
        void setModelViewMatrix(const Mat4 &mat);
        /// Returns the OpenGL model-view matrix.
        Mat4 modelViewMatrix() const;

        /// Set the active OpenGL viewport for drawing.
        void setViewport(int x, int y, int width, int height);
        /// Returns the active OpenGL viewport for drawing.
        Vec4i viewport() const;

        /// Set the near and far clip plane.
        void setClipPlanes(double near, double far);
        /// Returns the near clip plane.
        double near() const;
        /// Returns the far clip plane.
        double far() const;

        /// Apply viewport.
        void applyViewport();
        /// Apply projection matrix.
        void applyProjection(bool reset = true);
        /// Apply model-view matrix.
        void applyModelView(bool reset = true);
        /// Apply projection and model-view matrix and viewport.
        void apply();

        /// Fills internal viewport from current OpenGL state.
        void retrieveViewport();
        /// Fills internal projection matrix from current OpenGL state.
        void retrieveProjection();
        /// Fills internal model-view matrix from current OpenGL state.
        void retrieveModelView();
        /// Fills projection & model-view matrix and viewport from OpenGL state.
        void retrieve();

        /// Store current OpenGL state.
        void store();
        /// Restore current OpenGL state.
        void restore();

        /// Convert the OpenGL depth buffer to metric float values.
        void convertDepthBufferToMetric(cv::Mat &depth);

    private:
        Mat4 proj_mat_;
        Mat4 mv_mat_;
        Vec4i viewport_;
        double near_;
        double far_;
    };

} // namespace ogl
} // namespace menderer
