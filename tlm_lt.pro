TARGET = tlm_lt

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -Wno-unused-parameters -O3

systemc_home = $$(SYSTEMC_HOME)
isEmpty(systemc_home) {
    systemc_home = /opt/systemc
}

systemc_target_arch = $$(SYSTEMC_TARGET_ARCH)
isEmpty(systemc_target_arch) {
}

unix:!macx {
    systemc_target_arch = linux64
    QMAKE_CXXFLAGS += -std=c++11
}

macx: {
    systemc_target_arch = macosx64
    QMAKE_CXXFLAGS += -std=c++0x -stdlib=libc++
}

INCLUDEPATH += $${systemc_home}/include

LIBS += -L$${systemc_home}/lib-$${systemc_target_arch} -lsystemc

SOURCES += main.cpp

HEADERS += memory.h \
    bus.h
HEADERS += processor.h

OTHER_FILES += stimuli2.txt
OTHER_FILES += stimuli1.txt
