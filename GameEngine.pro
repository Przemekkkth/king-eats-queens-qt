greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

SOURCES += \
    src/fontmanager.cpp \
    src/gamescene.cpp \
    src/main.cpp \
    src/pixmapmanager.cpp \
    src/view.cpp

HEADERS += \
    src/player.h \
    src/fontmanager.h \
    src/gamescene.h \
    src/grass.h \
    src/pixmapmanager.h \
    src/queen.h \
    src/resourceholder.h \
    src/utils.h \
    src/view.h

RESOURCES += \
    resource.qrc
