QT       += core gui sql
# RC_ICONS = resources/FIXME.ico  // set application icon here.
TARGET = Yx-RoutineTracker

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    historydialog.cpp \
    historydialog_search.cpp \
    main.cpp \
    mainwindow.cpp \
    routinedialog.cpp \
    routinedialog_add.cpp

HEADERS += \
    historydialog.h \
    historydialog_search.h \
    mainwindow.h \
    routinedialog.h \
    routinedialog_add.h \
    routinestruct.h

FORMS += \
    historydialog.ui \
    historydialog_search.ui \
    mainwindow.ui \
    routinedialog.ui \
    routinedialog_add.ui

# Copies schema.sql to the build directory so it can be ran to create database if yx_routinetracker.db does not exist.
QMAKE_POST_LINK += \
    copy /Y \"$$PWD\\schema.sql\" \"$$OUT_PWD\\\" && \
    copy /Y \"$$PWD\\sqlite3.exe\" \"$$OUT_PWD\\\"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
