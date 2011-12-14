#------------------------------------------------------------------------------
# Linux specific project include
#------------------------------------------------------------------------------
BuildDir = build
InstallDir = install
ExeFile = $${TARGET}
AssetsDir = assets
DocsDir = docs
AssetsInstallDir = $${InstallDir}/$${AssetsDir}
DocsInstallDir = $${InstallDir}/$${DocsDir}


#------------------------------------------------------------------------------
# Definitions, include paths, libraries and resources
#------------------------------------------------------------------------------
DEFINES += LINUX

INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/local/include/libfreenect

LIBS += -L/usr/local/lib/ -lfreenect
LIBS += -L/usr/local/lib/ -lpng
LIBS += -L/usr/lib/ -lGLEW


#------------------------------------------------------------------------------
# Compiler flags
#------------------------------------------------------------------------------
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -O3 -funroll-loops -ftree-vectorize -Wno-unused
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -funroll-loops -ftree-vectorize -Wno-unused


#------------------------------------------------------------------------------
# Install target and assets.
#------------------------------------------------------------------------------
target.path += $${InstallDir}
INSTALLS += target

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
