QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    add_kotoba.cpp \
    folygonkire.cpp \
    main.cpp \
    mainqt.cpp \
    modify_kotoba.cpp \
    new_dic.cpp \
    sqlite3.c

HEADERS += \
    add_kotoba.h \
    folygonkire.h \
    mainqt.h \
    modify_kotoba.h \
    new_dic.h \
    sqlite3.h

FORMS += \
    add_kotoba.ui \
    mainqt.ui \
    modify_kotoba.ui \
    new_dic.ui

LIBS += -lSDL2 -lSDL2_mixer

TRANSLATIONS += \
    folygonkire_cpp_ja.ts \
    folygonkire_cpp_ko.ts \
    folygonkire_cpp_en.ts

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DEFINES += ko \
    windows
