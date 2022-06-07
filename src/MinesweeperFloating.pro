QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    gamecontroller.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    gamecontroller.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    MinesweeperFloating_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

RC_FILE = rc.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

win32:{
    CONFIG(debug, debug|release) {
        #debug
    } else {
        #release
        DESTDIR = $$_PRO_FILE_PWD_/../release_win32
        QMAKE_POST_LINK += $$(QTDIR)/bin/windeployqt --qmldir $$(QTDIR)/qml $$DESTDIR $$escape_expand(\\n\\t)
    }
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
