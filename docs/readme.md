# KFX (Kinect FX) Manual

Dominik Deak


## 1 System Requirements

### 1.1 Operating Systems

Supported operating systems include Windows XP, Vista and 7. Both 32-bit
and 64-bit platforms should work. Mac OS X is also supported and tested.
Linux should work, but was not tested.


### 1.2 Hardware

Kinect with an external power supply is required. Do not connect the device
directly to the USB port; use the power supply dongle provided. OpenGL 2.0
capable graphics card and driver is also required, including GLSL 1.20 support.
Additionally, the graphics card needs to support vertex texture image units
and the following extensions:

    ARB_framebuffer_object
    ARB_texture_float
    ARB_texture_non_power_of_two


## 2. Running the Binary Distribution

### 2.1 Windows Platforms

If you downloaded the pre-compiled version of KFX, you will need to install
the Kinect drivers provided in this archive. You should be able to locate them
in the `driver/inf` directory.


### 2.2 Mac OS X Platforms

If you downloaded the application bundle for KFX, you will need to build and
install the libfreenect Kinect drivers separately. For more information, visit
the link below.

<http://openkinect.org/wiki/Getting_Started#Manual_Build_under_OSX>


## 3. Source Code

The KFX project is based on the Qt SDK, a cross platform API. The source
code package is organised in the following way:

Path            | Description
---             | ---
assets/         | Contains shaders and textures
build/          | Intermediate files generated by the compiler
code/           | Source code and resource files
docs/           | Licenses and documentation
etc/            | Random files, templates, notes and incremental backup
install/        | Deployment files generated by the compiler
windows/        | Visual Studio 2010 project
kfx.linux.pri   | Linux project include for qmake
kfx.macosx.pri  | Mac OS X project include for qmake
kfx.pro         | Main project file for qmake
kfx.windows.pri | Windows project include for qmake

The entire project can be built with the aid of qmake, a tool provided by the
Qt SDK. Running qmake in the root directory (or more specifically, on `kfx.pro`)
will generate a set of makefiles suitable for your platform. The "build" and
`install` directories are generated by the qmake tool. The `etc` directory does
not participate in the build process. Details on running qmake will be given in
the subsequent sections.

There is a separate Visual Studio 2010 project contained in the `windows`
directory. The Visual Studio project is not intended for generating deployment
builds; its purpose is to develop and debug the project.


## 4 Compiling

### 4.1 Building on Windows

#### 4.1.1 Prerequisites on Windows

To build the project, you will need the following external libraries installed
on your machine:

* [Qt SDK for Windows](http://qt.nokia.com/downloads/downloads), based on 4.7.1 libraries
* [libpng 1.4](http://www.libpng.org)
* [zlib 1.2.5](http://www.zlib.net)
* [GLEW 1.5.7](http://glew.sourceforge.net)
* libfreenect, [unstable Win32 branch](https://github.com/slomp/libfreenect/tree/unstable), see also [Getting Started Windows](http://openkinect.org/wiki/Getting_Started_Windows)

The zlib library may be optional, depending on how libpng was configured.

The libfreenect project is a little bit tricky to get working. Follow
the instructions outlined in the links above. Kinect driver stability on
Windows is not exactly stellar; some systems may suffer from severe performance
degradation. Best case, you will see a good frame rate, with a few dropped frames.
Worst case, your application or operating system will seize up. Unfortunately,
stability issues will remain until a proper version of libusb 1.0 will be
ported to Windows (an essential component of libfreenect).

Once the packages are installed, make sure that `QTDIR` environment variable is
set, and points to the root path of the Qt SDK. Typically one must logout and
login again for the environment variable to take effect after installing Qt.

Additionally, the project makes references to the `SDKPATH` environment variable,
which points to the root directory where the rest of the prerequisite libraries
are located. This is optional, you may completely remove or alter references
to `SDKPATH` in the project files and use whatever path(s) appropriate to resolve
headers and libraries.


#### 4.1.2 Building on Windows with qmake (Recommended)

This is the preferred method for building a Windows project. Before commencing,
the aforementioned prerequisites must be installed on the machine. Visual Studio
2010 (Pro or Express) is also required. Alternate compilers, such as MinGW might
also work, but was not tested.

Running the qmake command in the project root directory should generate the
makefiles needed to build the application. Invoking qmake under the Visual Studio
Command Prompt environment is highly recommended.

The qmake project uses an operating system specific include file, `kfx.windows.pri`.
In there you will find settings pointing to various include and library paths for the
aforementioned prerequisites. Make sure these paths are correct before using qmake.
To generate the makefiles, run the command:

    qmake

The makefiles will have two build targets: One for `debug`, and one for `release`.
Intermediate files will be stored in the "build" subdirectory. To build the
application, use the following commands:

    nmake release

For debug targets:

    nmake debug

To consolidate files needed to for deployment, use this command:

    nmake /f makefile.release install

The above command will create an `install` subdirectory, containing a copy of
the executables, asset files and external DLL files. Note, one must be explicit in
specifying `/f makefile.release` option to make sure only the release build will be
added for deployment.

If the Qt SDK is a dynamic or a shared library, some external Qt dependencies must
be copied manually into the `install` subdirectory for deployment purposes. For more
information on this, consult the Qt documentation.

To clean the project, including files in `install`, use the command:

    nmake clean

Finally, to remove makefiles generated by qmake:

    nmake distclean


#### 4.1.3 Building on Windows with Visual Studio 2010

This project includes solution files generated by Visual Studio 2010, which was used
to write and to debug the entire project. You will find them under the "windows"
subdirectory. These project files were maintained by hand; therefore, any
changes to the project must be manually added. Those who are seeking an automated
build (for deployment purposes) should use the qmake method, as outlined in the
previous section.

Currently, the Visual Studio project references a dynamic build of Qt SDK.
If static Qt target is used, open the project Configuration Properties window, and
update the Additional Dependencies section (under Linker options) with the correct
library names.

Additionally, paths to various include and library directories (pointing to the
aforementioned prerequisites) must be updated to reflect the machine's current
install configuration. Some of these paths are prefixed by the `SDKPATH` environment
variable, which defines the root path where all the external libraries are located.
This can be altered or removed to suit your platform.

During the build process, Visual Studio will invoke various Qt tools to generate
headers and resources needed for the application. For that to happen, the `QTDIR`
environment variable must be defined and point to the Qt SDK root directory.


### 4.2 Building on Mac OS X

#### 4.2.1 Prerequisites on Mac OS X

To build the project, you will need the following external frameworks or libraries
installed on your machine:

* [Qt SDK for Windows](http://qt.nokia.com/downloads/downloads), based on 4.7.1 libraries
* [libpng 1.4](http://www.libpng.org)
* [GLEW 1.5.7](http://glew.sourceforge.net)
* libfreenect, see also [Getting Started](http://openkinect.org/wiki/Getting_Started#Manual_Build_under_OSX) manual


#### 4.2.2 Building on Mac OS X with qmake

This is the preferred method for building a Mac OS X project. Before commencing,
the aforementioned prerequisites must be installed on the machine. Running the
qmake tool in the project root directory should generate the makefiles needed
to build the application.

The qmake project uses an operating system specific include file, `kfx.macosx.pri`.
In there you will find settings pointing to various include and library paths for the
aforementioned prerequisites. Make sure these paths are correct before using qmake.
To generate the makefiles, run the command:

    qmake -spec macx-g++

The makefiles will have two build targets: One for `debug`, and one for `release`.
Intermediate files will be stored in the `build` subdirectory. To build the
application, use the following commands:

    make release

For debug targets:

    make debug

To consolidate files needed to for deployment, use this command:

    make -f Makefile.release install

The above command will create an `install` subdirectory, containing a copy of
the application bundle and asset files. Note, one must be explicit in specifying
`-f Makefile.release` option to make sure only the release build will be
added for deployment.

Before distributing the application bundle, you will need to check for external
dependencies used by the application. These dependencies must be included in the
bundle. The following tool allows checking for external dependencies:

    otool -L install/kfx.app/Contents/MacOS/kfx

There is a special step that must be taken on the deployment bundle. The qmake
project will manually copy `libGLEW.\*.dylib` into the bundle when you run the
`make -f Makefile.release install` command. However, the application binary
needs updating as well, so that it will search for the GLEW library inside the
bundle. This can be achieved by using the following tool:

    install_name_tool -change /usr/lib/libGLEW.1.5.7.dylib @executable_path/../Frameworks/libGLEW.1.5.7.dylib install/kfx.app/Contents/MacOS/kfx

Use the `otool` command mentioned earlier to check the exact version of the
`libGLEW.\*.dylib` file, and also used it after `install_name_tool` to make sure
the GLEW library has the `@executable_path/../Frameworks/` prefix.

You may have noticed that `otool` reveals additional dependencies, other than
GLEW. The Qt SDK provides a convenient tool that collects the rest of the
dependencies and copies them into the bundle:

    macdeployqt install/kfx.app

To create a compressed dmg file, use this command instead:

    macdeployqt install/kfx.app -dmg

To clean the project, including files in `install`, use the command:

    make clean

Finally, to remove makefiles generated by qmake:

    make distclean


## 5. Legal and Copyright

Please read [MIT License](./license-kfx.md) for terms and conditions.

KFX is copyright (C) 2011, [Bjango Pty Ltd](https://bjango.com), and [DEAK Software](https://deaksoftware.com.au). "Kinect", "XBox" and "XBox360" are trademarks owned by Microsoft Corporation.

