TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS+= -stdlib=libc++

SOURCES += main.cpp \
    ftpcontext.cpp \
    ftpsession.cpp \
    main.cpp \
    sessionmgr.cpp \
    state.cpp

HEADERS += \
    ftpcontext.h \
    ftpsession.h \
    sessionmgr.hpp \
    state.h \
    syncqueue.hpp \
    threadsafe_queue.h
