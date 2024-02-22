# GLVM

This is my simple game engine for Linux and Windows OS's with both Vulkan and Opengl support. Its based on entity component system (ECS) with user friendly C++ interface. Also it has partial support of GLTf and wavefront.obj 3D model formats. With GLVM you can make simple phong light of three types (directional, spot, point). Very basic physics included (collitions, gravity). 

## Linux
    
* ### Development libraries:

        X11, Xi, XRandR.

        Vulkan.

        Opengl.
    
        Alsa.

        pulseaudio.

* ### Repository specific:
* #### Gentoo:
        emerge --ask x11-libs/libX11 \
                     x11-libs/libXi \
                     x11-apps/xrandr \
                     media-libs/vulkan-loader \
                     dev-util/vulkan-tools \
                     media-libs/mesa \
                     media-libs/alsa-lib \
                     media-sound/pulseaudio

* #### Debian:
        apt install libx11-dev \
                    libxi-dev \
                    libxrandr-dev
                    libgl1-mesa-dev \
                    libasound2-dev \
                    libpulse-dev \
                    libudev-dev

* #### Arch:
        pacman -S libxi \
                  libxrandr \
                  mesa \
                  libglvnd \
                  alsa-lib \
                  pulseaudio

* #### Fedora:
        dnf install libX11-devel \
                    libXrandr-devel \
                    libXi-devel \
                    mesa-libGL-devel \
                    alsa-lib-devel \
                    pulseaudio-libs-devel \
                    libudev-devel \
                    libstdc++-static
