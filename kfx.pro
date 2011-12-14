#------------------------------------------------------------------------------
# Project configuration
#------------------------------------------------------------------------------
LANGUAGE = C++
QT += opengl
CONFIG += qt thread debug_and_release

#-- NOTE: The resource name must match the one used in code/source/main.cpp --
QMAKE_RESOURCE_FLAGS += -compress 9 -name kfx_resource


#------------------------------------------------------------------------------
# Platform specific stuff
#------------------------------------------------------------------------------
win32:include(kfx.windows.pri)
macx:include(kfx.macosx.pri)
linux:include(kfx.linux.pri)


#------------------------------------------------------------------------------
# Intermediate directories
#------------------------------------------------------------------------------
#-- NOTE: The opening brace must follow on the same line as the condition --
CONFIG(debug, debug|release) {
   DEFINES += DEBUG
   DESTDIR = $${BuildDir}/debug
   }

CONFIG(release, debug|release) {
   DEFINES += NDEBUG
   DESTDIR = $${BuildDir}/release
   }

OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc
UI_DIR = $${DESTDIR}/ui

message(---- Output Directories ----)
message($${DESTDIR})
message($${OBJECTS_DIR})
message($${MOC_DIR})
message($${RCC_DIR})
message($${UI_DIR})


#------------------------------------------------------------------------------
# Resources and forms
#------------------------------------------------------------------------------
RESOURCES += code/resource/*.qrc
FORMS += code/resource/*.ui


#------------------------------------------------------------------------------
# Header files
#------------------------------------------------------------------------------
HEADERS += code/source/*.h


#------------------------------------------------------------------------------
# Source files
#------------------------------------------------------------------------------
SOURCES += code/source/*.cpp
