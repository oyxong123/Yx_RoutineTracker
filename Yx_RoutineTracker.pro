QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    historysearchdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    routinedialog.cpp

HEADERS += \
    historysearchdialog.h \
    mainwindow.h \
    routinedialog.h

FORMS += \
    historysearchdialog.ui \
    mainwindow.ui \
    routinedialog.ui

# Copies schema.sql to the build directory so it can be ran to create database if yx_routinetracker.db does not exist.
QMAKE_POST_LINK += \
    copy /Y \"$$PWD\\schema.sql\" \"$$OUT_PWD\\\" && \
    copy /Y \"$$PWD\\sqlite3.exe\" \"$$OUT_PWD\\\"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
