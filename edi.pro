TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS+= -stdlib=libc++
INCLUDEPATH =/usr/include/c++/v1 /home/lmz/boost/include
LIBS += -L/usr/lib64 -L/home/lmz/boost/lib -lpthread -lc++ -lc++abi -lboost_system

SOURCES += main.cpp \
    ftpcontext.cpp \
    ftpsession.cpp \
    sessionmgr.cpp \
    state.cpp

HEADERS += \
    ftpcontext.h \
    ftpsession.h \
    sessionmgr.hpp \
    state.h \
    syncqueue.hpp \
    threadsafe_queue.h
