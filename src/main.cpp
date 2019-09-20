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

#include <menderer/mat.h>

#include <iomanip>
#include <iostream>
#include <memory>

#include <CLI/CLI.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <menderer/camera.h>
#include <menderer/dataset.h>
#include <menderer/mesh.h>
#include <menderer/mesh_util.h>
#include <menderer/ply_io.h>
#include <menderer/scene.h>
#include <menderer/trajectory.h>
#include <menderer/ogl/ogl.h>
#include <menderer/ogl/mesh_renderer.h>

/**
 * @brief   Menderer main application.
 *          Batch rendering of a 3D triangle mesh into the poses of a
 *          specified camera trajectory.
 * @author  Robert Maier
 */
int main(int argc, char *argv[])
{
    // parse command line arguments using CLI11
    CLI::App app("Menderer - Batch Mesh Renderer");

    // input files
    std::string cam_intrinsics_file;
    CLI::Option* opt_cam = app.add_option("-c,--camera", cam_intrinsics_file,
                                          "Camera intrinsics file")
            ->check(CLI::ExistingFile);
    std::string trajectory_file;
    CLI::Option* opt_traj = app.add_option("-t,--trajectory", trajectory_file, "Camera trajectory file (TUM RGB-D benchmark format)")
            ->check(CLI::ExistingFile)->needs(opt_cam);
    std::string dataset_folder;
    app.add_option("-d,--dataset", dataset_folder, "Dataset folder (Intrinsic3D format)")
            ->check(CLI::ExistingDirectory)->excludes(opt_traj);
    std::string mesh_file;
    app.add_option("-m,--mesh", mesh_file, "Input mesh file")
            ->required()->check(CLI::ExistingFile);

    // output folder
    std::string output_folder;
    app.add_option("-o,--output", output_folder, "Output folder")
            ->check(CLI::ExistingDirectory);

    // options for saving rendered depth as .png/.bin
    bool save_depth_png;
    app.add_flag("--save_depth_png", save_depth_png, "Save rendered depth (.png)");
    bool save_depth_bin;
    app.add_flag("--save_depth_binary", save_depth_bin, "Save rendered depth (binary)");

    // GUI parameters
    bool gui = false;
    app.add_flag("--gui", gui, "Show GUI");
    bool gui_pause = false;
    app.add_flag("--pause", gui_pause, "Pause after showing rendered frame");

    // renderer parameters
    menderer::ogl::MeshRenderer::Config renderer_cfg;

    // initialize and configure mesh scene
    // mesh color
    float color_r = 1.0f;
    app.add_option("--color_r", color_r, "Mesh color (red)");
    float color_b = 0.9f;
    app.add_option("--color_b", color_b, "Mesh color (blue)");
    float color_g = 0.75f;
    app.add_option("--color_g", color_g, "Mesh color (green)");
    // render background color
    float bg_r = 0.0f;
    app.add_option("--bg_r", bg_r, "Rendering background (red)");
    float bg_b = 0.0f;
    app.add_option("--bg_b", bg_b, "Rendering background (blue)");
    float bg_g = 0.0f;
    app.add_option("--bg_g", bg_g, "Rendering background (green)");
    // enable scene lighting
    renderer_cfg.lighting = false;
    app.add_flag("--lighting", renderer_cfg.lighting, "Enable lighting");
    // enable mesh colors
    renderer_cfg.colored = false;
    app.add_flag("--colored", renderer_cfg.colored, "Enable mesh colors");
    // configure flat/smooth rendering
    bool flat = false;
    app.add_flag("--flat", flat, "Enable flat rendering");
    // configure OpenGL shader
    // "none", "normals_phong", "phong"
    renderer_cfg.shader = "normals_phong";
    app.add_option("--shader", renderer_cfg.shader, "OpenGL rendering shader (default: normals_phong)");

    // parse command line arguments
    CLI11_PARSE(app, argc, argv);

    // fill and print renderer config
    renderer_cfg.color = menderer::Vec4f(color_r, color_b, color_g, 1.0f);
    renderer_cfg.background = menderer::Vec4f(bg_r, bg_b, bg_g, 1.0f);
    renderer_cfg.smooth = !flat;
    renderer_cfg.print();

    // create OpenGL context
    if (!menderer::ogl::createContext())
    {
        std::cerr << "could not create OpenGL context!" << std::endl;
        return 1;
    }

    // load dataset
    menderer::Dataset dataset;
    if (dataset_folder.empty())
    {
        // load camera intrinsics and trajectory
        if (!dataset.load(cam_intrinsics_file, trajectory_file))
        {
            std::cerr << "could not load dataset from files!" << std::endl;
            return 1;
        }
    }
    else
    {
        // load full Intrinsic3D dataset
        if (!dataset.load(dataset_folder))
        {
            std::cerr << "could not load Intrinsic3D dataset!" << std::endl;
            return 1;
        }
    }

    // print loaded camera intrinsics
    auto& camera = dataset.camera();
    camera.print();
    // check trajectory and print number of poses
    auto& trajectory = dataset.trajectory();
    if (trajectory.empty())
    {
        std::cerr << "trajectory is empty!" << std::endl;
        return 1;
    }
    //trajectory.print();
    std::cout << "trajectory: " << trajectory.size() << " poses" << std::endl;

    // load mesh from ply file
    menderer::Mesh mesh;
    if (!menderer::PlyIO::load(mesh_file, mesh))
    {
        std::cerr << "could not load mesh!" << std::endl;
        return 1;
    }
    mesh.print();

    if (mesh.normals.empty())
    {
        // compute mesh normals for rendering (if not present)
        std::cout << "compressing mesh vertices ..." << std::endl;
        menderer::MeshUtil::compressVertices(mesh);
        std::cout << "computing mesh normals ..." << std::endl;
        menderer::MeshUtil::computeVertexNormals(mesh);
        mesh.print();
    }

    // create and configure scene
    menderer::Scene scene(camera, renderer_cfg);
    // upload mesh to GPU
    scene.upload(mesh);

    if (gui)
    {
        // create windows for GUI mode
        if (dataset.hasColor())
            cv::namedWindow("input color");
        /*
        if (dataset.hasDepth())
            cv::namedWindow("input depth");
        */
        cv::namedWindow("rendered color");
        //cv::namedWindow("rendered depth");
    }

    // render mesh into target camera poses
    size_t num_frames = trajectory.size();
    std::cout << "rendering " << num_frames << " frames ..." << std::endl;
    for (size_t i = 0; i < num_frames; ++i)
    {
        std::cout << "   frame " << (i + 1) << " of " << num_frames << std::endl;

        // render mesh into current target pose
        menderer::Mat4 pose_world_to_cam = trajectory.pose(i).inverse();
        cv::Mat rendered_color, rendered_depth;
        if (!scene.render(pose_world_to_cam, rendered_color, rendered_depth))
        {
            std::cerr << "   could not render frame " << (i + 1) << "!" << std::endl;
            continue;
        }

        if (!output_folder.empty())
        {
            // store rendered frame
            std::stringstream ss;
            ss << output_folder;
            ss << "/render_" << std::setfill ('0') << std::setw(6) << i;
            std::string output_file_prefix = ss.str();

            // save rendered color
            std::string output_file_color = output_file_prefix + "-color.png";
            std::cout << "   saving color to " << output_file_color << " ..." << std::endl;
            menderer::Dataset::saveColor(output_file_color, rendered_color);

            // save rendered depth
            if (save_depth_png)
            {
                std::string output_file_depth_png = output_file_prefix + "-depth.png";
                std::cout << "   saving depth (.png) to " << output_file_depth_png << " ..." << std::endl;
                menderer::Dataset::saveDepthPNG(output_file_depth_png, rendered_depth);
            }
            if (save_depth_bin)
            {
                std::string output_file_depth_bin = output_file_prefix + "-depth.bin";
                std::cout << "   saving depth (.bin) to " << output_file_depth_bin << " ..." << std::endl;
                menderer::Dataset::saveDepthBinary(output_file_depth_bin, rendered_depth);
            }
        }

        if (gui)
        {
            // show frames in GUI mode

            // show rendered frame
            if (!rendered_color.empty())
                cv::imshow("rendered color", rendered_color);
            /*
            if (!rendered_depth.empty())
                cv::imshow("rendered depth", rendered_depth);
            */

            // show input frame
            if (dataset.hasColor())
            {
                // load and show input color image
                cv::Mat input_color = dataset.loadColor(i);
                if (!input_color.empty())
                    cv::imshow("input color", input_color);
            }
            /*
            if (dataset.hasDepth())
            {
                // load and show input depth map
                cv::Mat input_depth = dataset.loadDepth(i);
                if (!input_depth.empty())
                    cv::imshow("input depth", input_depth);
            }
            */

            int time_wait = gui_pause ? 0 : 30;
            int key = cv::waitKey(time_wait);
            if (key == 27)
                break;
        }

    }
    std::cout << "rendering finished (" << num_frames << " frames)" << std::endl;

    // clean up GUI
    if (gui)
        cv::destroyAllWindows();

    // destroy OpenGL context
    menderer::ogl::destroyContext();

    return 0;
}
