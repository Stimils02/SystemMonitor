TEMPLATE = app
TARGET = systemmonitor
QT += core gui widgets charts

# The following define the include and source paths
INCLUDEPATH += .

# Input
SOURCES += main.cpp \
           systemtab.cpp \
           processTab.cpp \
           resourceTab.cpp \
           fileSystemTab.cpp \
           CustomTableWidget.cpp \
           PropertiesWidget.cpp \
           MemoryMapsWidget.cpp \
           CustomTreeWidget.cpp \
           process.cpp \
           system_info.cpp \
           OpenFiles.cpp \
           graphs.cpp \
           InfoDialog.cpp
        

HEADERS += systemtab.h \
           processTab.h \
           resourceTab.h \
           fileSystemTab.h \
           CustomTableWidget.h \
           PropertiesWidget.h \
           MemoryMapsWidget.h \
           CustomTreeWidget.h \
           process.h \
           system_info.h \
           ProcessFilter.h \
           OpenFiles.h \
           graphs.h \
           ProcessFilter.h \
           InfoDialog.h \
           ContentType.h
