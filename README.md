# <img src="https://raw.githubusercontent.com/PQCraft/CaveCube/master/extras/icon/hicolor/128x128/apps/cavecube.png" align="right" height="95"/>CaveCube
**An in-development Minecraft/Infiniminer clone**<br>

---
[![](https://raw.githubusercontent.com/PQCraft/PQCraft/master/Screenshot_20220918_210819.png)](#?)

---
### Contributing
Please read the [contributing guide](https://github.com/PQCraft/CaveCube/blob/dev/CONTRIBUTING.md).<br>
The project status is written down in [TODO.md](https://github.com/PQCraft/CaveCube/blob/dev/TODO.md).<br>

---
### Requirements
#### Building
GLFW or SDL2, GNU Make, and a C compiler with POSIX thread.<br>
For Windows, the GLFW or SDL2 development binaries must be installed from their respective websites: [GLFW](https://www.glfw.org/download), [SDL2](https://www.libsdl.org/download-2.0.php).<br>
For outdated/stable Linux distributions (such as Debian or Ubuntu), GLFW must be built from source due to the package being out of date.<br>
#### Running
OpenGL 3.3 or OpenGLES 3.0 support.

---
### Bulding
- To build, run `make -j`.<br>
- To build and run once done compiling, run `make -j run`.<br>
- To build for debugging, add `DEBUG=[level]` after `make` (eg. `make DEBUG=0 -j run`). This will build the executable with debug symbols, disable symbol stripping, and define the internal `DEBUG` macro with the level specified.<br>
- To compile with using SDL2 instead of GLFW, add `USESDL2=y` after `make`.<br>
- To compile using OpenGL ES instead of OpenGL, add `USEGLES=y` after `make`.<br>
- To change the target to the standalone server, add `SERVER=y` after `make`. This variable will change the object directory.<br>
- To cross compile to Windows on a non-Windows OS, add `WINCROSS=y` after `make`. This variable will change the object directory and binary name.<br>
- To compile a 32-bit binary on a 64-bit machine, add `M32=y` after `make`. This variable will change the object directory.<br>

These variables can be combined.<br>
For example, `make USESDL2=y WINCROSS=y -j run` will build CaveCube for Windows with SDL2 and run when compilation finishes.<br>
<br>
For any variables that change the object directory or binary name, you must use these flags again when running the `clean` rule in order to remove the correct files.<br>

---
### Notes <img src="https://repology.org/badge/vertical-allrepos/cavecube.svg" alt="Packaging status" align="right"><br>
- CaveCube can be installed on Arch Linux using the [cavecube](https://aur.archlinux.org/packages/cavecube) or [cavecube-bin](https://aur.archlinux.org/packages/cavecube-bin) package.<br>
