QT -= gui

TEMPLATE = lib
DEFINES += ESTRIPSOLVER_LIBRARY

CONFIG += c++14

QMAKE_CFLAGS_RELEASE -= -O1
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -m64 -O3
QMAKE_CXXFLAGS_RELEASE += -m64 -O3

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32: LIBS += -L$$PWD/../../eGenetic/build-Release/release/ -leGenetic
else:unix:!macx: LIBS += -L$$PWD/../../.eGenetic/build-Release/ -leGenetic

win32: INCLUDEPATH += $$PWD/../../eGenetic/eGenetic/include
else:unix:!macx: INCLUDEPATH += $$PWD/../../.eGenetic/eGenetic/include
win32: DEPENDPATH += $$PWD/../../eGenetic/eGenetic/include
else:unix:!macx: INCLUDEPATH += $$PWD/../../.eGenetic/eGenetic/include

HEADERS += \
    include/eStripSolver_global.h \
    include/egeneticcalculator.h \
    src/GuillotineBinPack.h \
    src/MaxRectsBinPack.h \
    src/Rect.h \
    src/StripPacking.h \
    src/egeneticresults.h \
    src/esize.h \
    src/estripsolve.h

SOURCES += \
    src/GuillotineBinPack.cpp \
    src/MaxRectsBinPack.cpp \
    src/Rect.cpp \
    src/egeneticcalculator.cpp \
    src/egeneticresults.cpp \
    src/esize.cpp \
    src/estripsolve.cpp
