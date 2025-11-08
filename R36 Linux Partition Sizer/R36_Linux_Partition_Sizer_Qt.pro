QT += widgets \
    dbus \
    gui
CONFIG += c++11
SOURCES += main.cpp mainwindow.cpp \
    frmabout.cpp \
    frmhelpandinfo.cpp
HEADERS += mainwindow.h \
    frmabout.h \
    frmhelpandinfo.h
FORMS += mainwindow.ui \
    frmabout.ui \
    frmhelpandinfo.ui
RESOURCES += resources.qrc
TARGET = XNLR36LinuxPartitionSizer
