TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/local/include \
               /Users/lmz/Downloads/st-1.9/DARWIN_17.2.0_DBG

LIBS += -L/usr/local/lib \
        -L/Users/lmz/Downloads/st-1.9/DARWIN_17.2.0_DBG \
        -ljemalloc \
        -lst

SOURCES += filestream.cpp \
    ftpclient.cpp \
    main.cpp \
    socketstream.cpp \
    stcondition.cpp \
    stmutex.cpp \
    ststreambuf.cpp \
    utilities.cpp \
    threadpool.cpp

HEADERS += \
    filestream.hpp \
    ftpclient.h \
    guard.h \
    noncopyable.hpp \
    socketstream.hpp \
    stcondition.h \
    stmutex.h \
    ststreambuf.hpp \
    stsyncqueue.hpp \
    utilities.hpp \
    istream.h \
    threadpool.h
