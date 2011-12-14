#------------------------------------------------------------------------------
# Windows specific project include
#------------------------------------------------------------------------------
BuildDir = build
InstallDir = install
ExeFile = $${TARGET}.exe
DriversDir = drivers
AssetsDir = assets
DocsDir = docs
DriversInstallDir = $${InstallDir}/$${DriversDir}
AssetsInstallDir = $${InstallDir}/$${AssetsDir}
DocsInstallDir = $${InstallDir}/$${DocsDir}


#------------------------------------------------------------------------------
# Obtain environment variables for the Qt root directory, and the root path
# where the prerequisite libraries are located. Note, you may need to edit the
# individual INCLUDEPATH and LIBS entries to reflect your machine's 
# configuration.
#------------------------------------------------------------------------------
QtDir = $$(QTDIR)          #Qt root path
SdkDir = $$(SDKPATH)       #Root location for prerequisite libraries

message(---- Library Directories ----)
message($${QtDir})
message($${SdkDir})


#------------------------------------------------------------------------------
# Definitions, include paths and resources
#------------------------------------------------------------------------------
DEFINES += _WINDOWS
DEFINES += WINDOWS
DEFINES += WIN32
DEFINES += UNICODE

INCLUDEPATH += $$quote($${SdkDir}/glew/include)
INCLUDEPATH += $$quote($${SdkDir}/libfreenect/include)
INCLUDEPATH += $$quote($${SdkDir}/libpng)
INCLUDEPATH += $$quote($${SdkDir}/zlib)

RC_FILE = code/resource/resource.rc


#------------------------------------------------------------------------------
# Libraries, binaries, and compiler flags for debug build
#------------------------------------------------------------------------------
CONFIG(debug, debug|release) {
   LIBS += $$quote($${SdkDir}/glew/lib32/glew32.lib)
   LIBS += $$quote($${SdkDir}/libfreenect/lib/debug/freenect.lib)
   LIBS += $$quote($${SdkDir}/libpng/lib/win32/dll debug/libpng14d.lib)
   LIBS += $$quote($${SdkDir}/zlib/lib/win32/dll debug/zlib1d.lib)

   InstallBinary.path += $${InstallDir}
   InstallBinary.files += $$quote($${SdkDir}/glew/bin32/glew32.dll)
   InstallBinary.files += $$quote($${SdkDir}/libfreenect/lib/debug/freenect.dll)
   InstallBinary.files += $$quote($${SdkDir}/libpng/lib/win32/dll debug/libpng14d.dll)
   InstallBinary.files += $$quote($${SdkDir}/zlib/lib/win32/dll debug/zlib1d.dll)
   InstallBinary.files += $$quote($${SdkDir}/glew/bin32/glew32.dll)
   InstallBinary.files += $$quote($${QtDir}/bin/qtguid4.dll)
   InstallBinary.files += $$quote($${QtDir}/bin/qtcored4.dll)
   InstallBinary.files += $$quote($${QtDir}/bin/qtopengld4.dll)
   InstallBinary.files += $$quote($${QtDir}/bin/qtsvgd4.dll)
   InstallBinary.files += $$quote($${QtDir}/bin/qtxmld4.dll)
   INSTALLS += InstallBinary

   InstallPlugin.path += $${InstallDir}/plugins/imageformats
   InstallPlugin.files += $$quote($${QtDir}/plugins/imageformats/qgifd4.dll)
   InstallPlugin.files += $$quote($${QtDir}/plugins/imageformats/qicod4.dll)
   InstallPlugin.files += $$quote($${QtDir}/plugins/imageformats/qjpegd4.dll)
   InstallPlugin.files += $$quote($${QtDir}/plugins/imageformats/qmngd4.dll)
   InstallPlugin.files += $$quote($${QtDir}/plugins/imageformats/qsvgd4.dll)
   InstallPlugin.files += $$quote($${QtDir}/plugins/imageformats/qtiffd4.dll)
   INSTALLS += InstallPlugin

   QMAKE_CXXFLAGS_MT_DLLDBG += /MDd
   }


#------------------------------------------------------------------------------
# Libraries, binaries, and compiler flags for release build
#------------------------------------------------------------------------------
CONFIG(release, debug|release) {
   LIBS += $$quote($${SdkDir}/glew/lib32/glew32.lib)
   LIBS += $$quote($${SdkDir}/libfreenect/lib/release/freenect.lib)
   LIBS += $$quote($${SdkDir}/libpng/lib/win32/dll release/libpng14.lib)
   LIBS += $$quote($${SdkDir}/zlib/lib/win32/dll release/zlib1.lib)

   InstallBinary.path += $${InstallDir}
   InstallBinary.files += $$quote($${SdkDir}/glew/bin32/glew32.dll)
   InstallBinary.files += $$quote($${SdkDir}/libfreenect/lib/release/freenect.dll)
   InstallBinary.files += $$quote($${SdkDir}/libpng/lib/win32/dll release/libpng14.dll)
   InstallBinary.files += $$quote($${SdkDir}/zlib/lib/win32/dll release/zlib1.dll)
   InstallBinary.files += $$quote($${QtDir}/bin/qtgui4.dll)
   InstallBinary.files += $$quote($${QtDir}/bin/qtcore4.dll)
   InstallBinary.files += $$quote($${QtDir}/bin/qtopengl4.dll)
   InstallBinary.files += $$quote($${QtDir}/bin/qtsvg4.dll)
   InstallBinary.files += $$quote($${QtDir}/bin/qtxml4.dll)
   INSTALLS += InstallBinary

   InstallPlugin.path += $${InstallDir}/plugins/imageformats
   InstallPlugin.files += $$quote($${QtDir}/plugins/imageformats/qgif4.dll)
   InstallPlugin.files += $$quote($${QtDir}/plugins/imageformats/qico4.dll)
   InstallPlugin.files += $$quote($${QtDir}/plugins/imageformats/qjpeg4.dll)
   InstallPlugin.files += $$quote($${QtDir}/plugins/imageformats/qmng4.dll)
   InstallPlugin.files += $$quote($${QtDir}/plugins/imageformats/qsvg4.dll)
   InstallPlugin.files += $$quote($${QtDir}/plugins/imageformats/qtiff4.dll)
   INSTALLS += InstallPlugin

   QMAKE_CXXFLAGS_MT_DLL += /MD
   QMAKE_CFLAGS_RELEASE -= /Od /O1 /O2
   QMAKE_CFLAGS_RELEASE += /Ox /Ob2 /Oi /Ot /Oy /GT /GL /GF /Zp16 /Gy- /fp:fast /fp:except- /arch:SSE2 /MP
   QMAKE_CXXFLAGS_RELEASE -= /Od /O1 /O2
   QMAKE_CXXFLAGS_RELEASE += /Ox /Ob2 /Oi /Ot /Oy /GT /GL /GF /Zp16 /Gy- /fp:fast /fp:except- /arch:SSE2 /MP
   }


#------------------------------------------------------------------------------
# Install target and assets.
#------------------------------------------------------------------------------
target.path += $${InstallDir}
INSTALLS += target

InstallDrivers.path += $${DriversInstallDir}
InstallDrivers.files += $$quote(\"$${SdkDir}/libfreenect/inf\")
INSTALLS += InstallDrivers

InstallAssets.path += $${AssetsInstallDir}
InstallAssets.files += $${AssetsDir}/*
INSTALLS += InstallAssets

InstallDocs.path += $${DocsInstallDir}
InstallDocs.files += $${DocsDir}/*
INSTALLS += InstallDocs


#------------------------------------------------------------------------------
# Clean install files
#------------------------------------------------------------------------------
QMAKE_CLEAN += $$quote(/q /s $${InstallDir}\\*)
