#-------------------------------------------------
#
# Project created by QtCreator 2015-11-14T22:20:23
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Huffman
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    huffman.cpp

DISTFILES += \
    in.txt

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -O2
    win32 {
        TARGET_PATH = $${OUT_PWD}/release/$${TARGET}.exe
        TARGET_PATH ~= s,/,\\,g

        OUT_PATH = $${OUT_PWD}/
        OUT_PATH ~= s,/,\\,g

        REMOVE_DIRS += $${OUT_PWD}/release
        REMOVE_DIRS += $${OUT_PWD}/debug
        REMOVE_FILES += $${OUT_PWD}/Makefile
        REMOVE_FILES += $${OUT_PWD}/Makefile.Debug
        REMOVE_FILES += $${OUT_PWD}/Makefile.Release
        REMOVE_FILES += $${OUT_PWD}/object_script.$${TARGET}.Release
        REMOVE_FILES += $${OUT_PWD}/object_script.$${TARGET}.Debug
        REMOVE_DIRS ~= s,/,\\,g
        REMOVE_FILES ~= s,/,\\,g

        QMAKE_LFLAGS += -static
        QMAKE_LFLAGS += -static-libgcc
        QMAKE_LFLAGS += -static-libstdc++
        QMAKE_POST_LINK += 'move $${TARGET_PATH} $${OUT_PWD} \
            && rmdir /s /q $${REMOVE_DIRS} \
            && del $${REMOVE_FILES}'
    }
}
CONFIG(debug, debug|release) {
    win32 {
        QMAKE_LFLAGS += -static
        QMAKE_LFLAGS += -static-libgcc
        QMAKE_LFLAGS += -static-libstdc++
    }
}
