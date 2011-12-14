#------------------------------------------------------------------------------
# Mac OS X specific project include
#------------------------------------------------------------------------------
BuildDir = build
InstallDir = install
ExeFile = $${TARGET}.app
AssetsDir = assets
DocsDir = docs
FrameworksInstallDir = $${InstallDir}/$${ExeFile}/Contents/Frameworks
AssetsInstallDir = $${InstallDir}/$${ExeFile}/Contents/Resources/$${AssetsDir}
DocsInstallDir = $${InstallDir}/$${ExeFile}/Contents/Resources/$${DocsDir}


#------------------------------------------------------------------------------
# Definitions, include paths, libraries and resources
#------------------------------------------------------------------------------
DEFINES += MACOSX

INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/local/include/libfreenect

LIBS += -L/usr/local/lib/ -lfreenect
LIBS += -L/usr/local/lib/ -lpng
LIBS += -L/usr/lib/ -lGLEW

ICON = code/resource/logo.icns


#------------------------------------------------------------------------------
# Compiler flags
#------------------------------------------------------------------------------
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -O3 -funroll-loops -ftree-vectorize -Wno-unused
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -funroll-loops -ftree-vectorize -Wno-unused


#------------------------------------------------------------------------------
# Install target and assets. Note, target must be added first, as it overwrites
# assets within the bundle. Some dylibs are also manually copied into the
# bundle, so make sure their paths and file names are correct.
#------------------------------------------------------------------------------
target.path += $${InstallDir}
INSTALLS += target

InstallBinary.path += $${FrameworksInstallDir}
InstallBinary.files += /usr/lib/libGLEW.*.dylib
InstallBinary.files += /usr/local/lib/libfreenect.*.dylib
INSTALLS += InstallBinary

InstallAssets.path += $${AssetsInstallDir}
InstallAssets.files += $${AssetsDir}/*
INSTALLS += InstallAssets

InstallDocs.path += $${DocsInstallDir}
InstallDocs.files += $${DocsDir}/*
INSTALLS += InstallDocs


#------------------------------------------------------------------------------
# Clean install files
#------------------------------------------------------------------------------
QMAKE_CLEAN += $$quote(-r $${AssetsInstallDir})
QMAKE_CLEAN += $$quote(-r $${DocsInstallDir})
QMAKE_CLEAN += $$quote(-r $${InstallDir})
