TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -ljemalloc -lst

SOURCES += edi/filestream.cpp \
    edi/ftpclient.cpp \
    edi/main.cpp \
    edi/socketstream.cpp \
    edi/stcondition.cpp \
    edi/stmutex.cpp \
    edi/ststreambuf.cpp \
    edi/utilities.cpp

HEADERS += \
    edi/filestream.hpp \
    edi/ftpclient.h \
    edi/guard.h \
    edi/noncopyable.hpp \
    edi/socketstream.hpp \
    edi/stcondition.h \
    edi/stmutex.h \
    edi/ststreambuf.hpp \
    edi/stsyncqueue.hpp \
    edi/utilities.hpp
