# chipgl - GLSL live coding for your [Pocket C.H.I.P.](https://docs.getchip.com/index.html)

## Installation
1. Clone the repo somewhere on your device
1. Install cmake if you haven't already - `sudo apt-get install cmake`
1. Make a build directory and go into it. `mkdir build && cd build`
1. Compile the project. `cmake .. && make`
1. Run the project. `./chipgl`

## Usage
This applications is intended to be a frag shader playground. It will watch for changes on the vertex or fragment shaders and
recompile them on the fly. The recommended workflow for live-reloading is to set up your Pocket C.H.I.P. on your desk or table 
and then ssh into it from another computer to edit the shaders.

Alternately, when on the go, just flip between editing the shader in the terminal and running the application again.

Currently, the only uniforms exposed to the shaders are `vec2 resolution` and `float time`.

## Caveats
I initially tried using `rmate` with Atom to edit shaders and it introduced some async/concurrency issues with shader recompiling.
I recommend to directly ssh into the device and edit the shaders in the terminal if you want live reload.

## Thanks
This application makes use of [eglo](https://github.com/thp/eglo) as a wrapper over X11 and some gunky OpenGL nastiness. Go check it out!
