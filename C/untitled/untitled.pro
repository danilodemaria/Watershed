TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \
    pgmfiles.c \
    pgmtolist.c \
    watershed.c

HEADERS += \
    pgmfiles.h \
    watershed.h
