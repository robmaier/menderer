# Menderer - Batch Mesh Renderer

**Menderer** is a simple OpenGL mesh renderer for batch rendering a 3D triangle mesh into a list of specified camera poses.
It supports loading of meshes from [PLY (ascii or binary) files](https://en.wikipedia.org/wiki/PLY_(file_format)).
While various shadings (e.g. [Phong shading](https://en.wikipedia.org/wiki/Phong_shading)) are implemented, there is currently no support for texture maps or normal/displacement maps.
The tool is written in C++ and can be executed as a standalone commandline application.

If you find the Menderer source code useful in your research or project, please feel free to cite it as follows:
```
@misc{menderer,
  title = {Menderer - Batch Mesh Renderer},
  author = {Maier, Robert},
  howpublished = "\url{https://github.com/robmaier/menderer}",
  year={2019}
}
```


## Installation
The code was mostly developed and tested on Ubuntu Linux, hence we only provide the build instructions for Ubuntu in the following.
However, the code should also work on other platforms.

Please first clone the source code:
```
git clone https://github.com/robmaier/menderer.git
```

### Dependencies
Menderer requires
[CMake](https://cmake.org/download/),
[OpenCV 3](https://opencv.org/releases.html),
[Eigen](http://eigen.tuxfamily.org/),
[CLI11](https://github.com/CLIUtils/CLI11/),
[GLFW3](https://www.glfw.org/),
[happly](https://github.com/nmwsharp/happly)
as third-party dependencies.
While Eigen, CLI11 and GLFW3 are already contained in the ```third_party``` folder, the other dependencies can be installed directly from the default Ubuntu repositories:
```
sudo apt install cmake libopencv-dev
```

### Build Menderer
To compile the Menderer application, use the standard CMake approach:
```
mkdir build
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j6
```

## Run Menderer
In the following, we provide a simple usage example for the data ```data/lion/```.
First, an output folder ```output/``` for the renderings is created:
```
cd ../data/lion/
mkdir output/
```

We render the mesh ```mesh.ply``` into a virtual camera, specified by the pinhole camera parameters in ```intrinsics.txt``` and the target pose in ```pose.txt```:
```
../../build/bin/Menderer -c intrinsics.txt -t pose.txt -m mesh.ply -o output/
```

Instead of having only a single target camera pose, it is also possible to render the mesh into a list of camera poses ```trajectory.txt``` ([TUM RGB-D Benchmark trajectory format](https://vision.in.tum.de/data/datasets/rgbd-dataset/file_formats)):
```
../../build/bin/Menderer -c intrinsics.txt -t trajectory.txt -m mesh.ply -o output/
```
The output images are generated in the ```output/``` subfolder.


### Mesh Format
Since Menderer can only load meshes from .ply files, we recommend [Meshlab](http://www.meshlab.net/) for converting meshes in other formats (e.g. .obj, .wrl, etc.) to the .ply format.

### Intrinsic3D Format
In addition to the format for camera intrinsics (```intrinsics.txt```) and poses (```pose.txt``` / ```trajectory.txt```) above, we also support the [Intrinsic3D dataset format](https://vision.in.tum.de/data/datasets/intrinsic3d).
We show an example for downloading and rendering the ```Tomb Statuary``` data in the following:

```
# cd into data/ folder again
cd ../
# create folder for dataset
mkdir tomb
cd tomb/
# create output/ folder
mkdir output/

# download, extract and rename mesh
wget https://vision.in.tum.de/_media/data/datasets/intrinsic3d/tomb-statuary-intrinsic3d.zip
unzip tomb-statuary-intrinsic3d.zip
mv tomb-statuary-intrinsic3d mesh.ply

# download, extract and rename rgbd (with intrinsics and poses)
wget https://vision.in.tum.de/_media/data/datasets/intrinsic3d/tomb-statuary-rgbd.zip
unzip tomb-statuary-rgbd.zip
mv tomb-statuary-rgbd rgbd
```

After preparing the dataset, we can batch-render the mesh into the provided input camera poses:
```
../../build/bin/Menderer -d rgbd/ -m mesh.ply -o output/
```


### Command line arguments


```
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
```

    

### Load exported depth in Matlab

load binary depth in matlab

```
% read file into matrix A
fileID = fopen('out_0.bin');
A = fread(fileID,[640, 480],'float');
fclose(fileID);

% permute dimensions (account for row-major/column-major)
A = permute(A,[2 1]);

% convert A to grayscale image
I = mat2gray(A);

% show image
imshow(I);
```



## License
The Menderer source code is licensed under the [GNU General Public License Version 3 (GPLv3)](http://www.gnu.org/licenses/gpl.html), please see the [LICENSE](LICENSE) file for details.


## Contact
If you have any questions, please contact [Robert Maier &lt;robert.maier@tum.de>](mailto:robert.maier@tum.de).
