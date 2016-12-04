# Installation
Build using cmake. The easiest way is to use the gui, where you can select the
algorithms that you want to include in the build, and the location to which
you want your project files saved. 

Be sure to select a proper CMAKE_INSTALL_PREFIX (especially on
windows; on linux the default is /usr/local/ - you probably don't want to
install there, either, as this is just a test library you'll probably forget
about).

If you want the production version (i.e. stripped symbols), you should turn
HAMMING_BUILD_TESTS OFF.

You can also select the build type: Release or Debug (or both, if you're using
MSVC).

Ok, now configure the thing, and generate the project files for your favorite
IDE. On Windows that's probably Visual Studio, on Linux, makefiles. If you're
on linux, go to the build directory and run make install. If you're using MSVC,
open the solution in the build folder, and build the project named CMAKE_INSTALL.
This step should make sure that all the binaries (and lib's includes) are in 
CMAKE_INSTALL_PREFIX.

For using the exe, go to CMAKE_INSTALL_PREFIX/bin and execute it . Note that you
need to add <install path>/lib to the executable's loader paths before being 
able to run (you can do this via LD_CONFIG_PATH on linux, or by specifying 
the environemnt variables in the debug options of your executable project in 
MSVC) on linux you could do something along the lines of

hamming_install/bin$ LD_LIBRARY_PATH=../lib ./hamming

To execute the tests, run the hamming_test executable in <install path>/bin,
the same way you would run the executable.
 
If you want another executable (client module) to link to the library, the
easiest way to pass the relevant precompiler definitions is by using cmake. 

In the client's CMakeLists.txt, you just need to write

find_package(hamming)
add_executable(<my_executable_name> ...)
target_link_libraries(<my_executable_name> hamming)

and set the hamming path to the install path that you specified when building.

That's it.