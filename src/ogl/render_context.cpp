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

#include <menderer/ogl/render_context.h>

#include <iostream>


namespace menderer
{
namespace ogl
{

    RenderContext::RenderContext() :
        proj_mat_(Mat4::Identity()),
        mv_mat_(Mat4::Identity()),
        viewport_(0, 0, 0, 0),
        near_(0.1),
        far_(5.0)
    {
    }


    RenderContext::~RenderContext()
    {
    }


    void RenderContext::setPinholeProjection(int width, int height, const Mat3 &K)
    {
        double fx = K(0, 0);
        double fy = K(1, 1);
        double cx = K(0, 2);
        double cy = K(1, 2);
        setPinholeProjection(width, height, fx, fy, cx, cy);
    }


    void RenderContext::setPinholeProjection(int width, int height, double fx, double fy, double cx, double cy)
    {
        // perspective projection
        bool invertY = false;
        double inv = invertY ? -1.0 : 1.0;
        proj_mat_ <<    2.0 * fx / width, 0.0, (width - 2.0*cx) / width, 0.0,
                        0.0, inv * -2.0 * fy / height, inv * (height - 2.0 * cy) / height, 0.0,
                        0.0, 0.0, (-far_ - near_) / (far_ - near_), -2.0 * far_ * near_ / (far_ - near_),
                        0.0, 0.0, -1.0, 0.0;
    }


    Mat4 RenderContext::projectionMatrix() const
    {
        return proj_mat_;
    }


    void RenderContext::setModelViewMatrix(const Mat4 &mat)
    {
        mv_mat_ = mat;

        // rotate 180 degrees around x-axis to have z-axis point away from virtual camera
        mv_mat_.row(1) *= -1.0;
        mv_mat_.row(2) *= -1.0;
    }


    Mat4 RenderContext::modelViewMatrix() const
    {
        return mv_mat_;
    }


    void RenderContext::setViewport(int x, int y, int width, int height)
    {
        viewport_[0] = x;
        viewport_[1] = y;
        viewport_[2] = width;
        viewport_[3] = height;
    }


    Vec4i RenderContext::viewport() const
    {
        return viewport_;
    }


    void RenderContext::setClipPlanes(double near, double far)
    {
        near_ = near;
        far_ = far;
    }


    double RenderContext::near() const
    {
        return near_;
    }


    double RenderContext::far() const
    {
        return far_;
    }


    void RenderContext::applyViewport()
    {
        glViewport(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);
    }


    void RenderContext::applyProjection(bool reset)
    {
        glMatrixMode(GL_PROJECTION);
        if (reset)
            glLoadIdentity();
        glMultMatrixd(proj_mat_.data());
    }


    void RenderContext::applyModelView(bool reset)
    {
        glMatrixMode(GL_MODELVIEW);
        if (reset)
            glLoadIdentity();
        glMultMatrixd(mv_mat_.data());
    }


    void RenderContext::apply()
    {
        applyViewport();
        applyProjection();
        applyModelView();
    }


    void RenderContext::retrieveViewport()
    {
        // retrieve current viewport
        glGetIntegerv(GL_VIEWPORT, viewport_.data());
    }


    void RenderContext::retrieveProjection()
    {
        // retrieve current projection matrix
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixd(proj_mat_.data());
    }


    void RenderContext::retrieveModelView()
    {
        // retrieve current modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixd(mv_mat_.data());
    }


    void RenderContext::retrieve()
    {
        retrieveViewport();
        retrieveProjection();
        retrieveModelView();
    }


    void RenderContext::store()
    {
        // store viewport (and enable bit)
        glPushAttrib(GL_VIEWPORT_BIT | GL_ENABLE_BIT);
        // store projection matrix
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        // store modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
    }


    void RenderContext::restore()
    {
        // restore modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        // restore projection matrix
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        // restore viewport (and enable bit)
        glPopAttrib();
    }


    void RenderContext::convertDepthBufferToMetric(cv::Mat &depth)
    {
        int w = depth.cols;
        int h = depth.rows;
        cv::Mat depth_out = cv::Mat::zeros(h, w, depth.type());
        float n = static_cast<float>(near_);
        float f = static_cast<float>(far_);

        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                float d = depth.at<float>(y, x);

                float d_scaled;
                if (d == 1.0f)
                {
                    d_scaled = std::numeric_limits<float>::quiet_NaN();
                }
                else
                {
                    // scale depth buffer value to real units
                    // perspective projection
                    float zn = 2.0f * d - 1.0f;
                    d_scaled = (2.0f * n * f) / (f + n - zn * (f - n));
                }

                depth_out.at<float>(y, x) = d_scaled;
            }
        }

        depth = depth_out;
    }

} // namespace ogl
} // namespace menderer
