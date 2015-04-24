include(db_info.pri)

DEFINES += KIWI_PORT=6280
DEFINES += KIWI_HOSTNAME=localhost

DEFINES += NATIVE_RENDERING

DEFINES += DEBUG_SPEED
DEFINES += DEBUG_CHAR_STREAM
#DEFINES += DEBUG_PRINT_CONTENT
#DEFINES += DEBUG_LOOKUP
#DEFINES += DEBUG_PARSER
#DEFINES += DEBUG_STREAM
#DEFINES += DEBUG_TAG_PARSER
#DEFINES += DEBUG_TOC_PARSER
#DEFINES += DEBUG_TOC_RENDER
#DEFINES += DEBUG_STYLE_PARSER
#DEFINES += DEBUG_STYLE_DETECT

QT += core
QT += network
QT += sql
QT += xml
QT -= gui

TARGET = kiwi
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

#UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj

QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -Wno-deprecated-register
#QMAKE_CXXFLAGS += -Wno-enum-compare
#QMAKE_CXXFLAGS += -Wno-unused-function

#QMAKE_LFLAGS += -Lsrc/parser -Bstatic -lparse -Wl,-Bdynamic

INCLUDEPATH += src/soloutils
INCLUDEPATH += src/service
INCLUDEPATH += src
INCLUDEPATH += /usr/include/lua5.2

QMAKE_CXXFLAGS += `pkg-config --cflags lua5.2`
QMAKE_LFLAGS += `pkg-config --libs lua5.2`

include(src/service/qtservice.pri)
#include(src/qhttp/qhttp.pri)

#include(src/soloutils/tools/tools.pri)
#include(src/soloutils/kernel/kernel.pri)


SOURCES += src/main.cpp \
    src/util/dbinterface.cpp \
    src/netresponse/article.cpp \
    src/httpio/http_daemon.cpp \
    src/httpio/http_server.cpp \
    src/netresponse/file.cpp \
    src/httpio/request_template.cpp \
    src/netresponse/download.cpp \
    src/util/filecache.cpp \
    src/parse/char_stream.cpp \
    src/parse/container_stream.cpp \
    src/parse/allowed_tags.cpp \
    src/parse/parser_mode.cpp \
    src/util/queries.cpp \
    src/soloutils/hustring.cpp \
    src/util/dombuilder.cpp \
    src/util/wikidbinterface.cpp \
    src/htmlgen/code.cpp \
    src/htmlgen/element.cpp \
    src/htmlgen/encode.cpp \
    src/htmlgen/toc.cpp \
    src/htmlgen/lua_sandbox.cpp \
    src/htmlgen/page.cpp \
    src/htmlgen/module/image.cpp \
    src/htmlgen/module/module.cpp \
    src/htmlgen/module/template.cpp \
    src/netresponse/response_relay.cpp \
    src/htmlgen/user_controls.cpp \
    src/httpio/http_header.cpp \
    src/soloutils/ustring.cpp \
    src/htmlgen/wikieditor.cpp

HEADERS += \
    src/util/dbinterface.h \
    src/netresponse/article.h \
    src/httpio/http_server.h \
    src/httpio/http_daemon.h \
    src/netresponse/file.h \
    src/httpio/request_template.h \
    src/netresponse/download.h \
    src/util/filecache.h \
    src/util/queries.h \
    src/parse/char_stream.h \
    src/parse/container_stream.h \
    src/parse/allowed_tags.h \
    src/parse/parser_mode.h \
    src/globalvars.h \
    src/util/nullable.h \
    src/soloutils/hustring.h \
    src/soloutils/soloutils.h \
    src/soloutils/containers.h \
    src/soloutils/hashing.h \
    src/soloutils/error_reporting.h \
    src/soloutils/basic_type.h \
    src/util/dombuilder.h \
    src/util/wikidbinterface.h \
    src/htmlgen/code.h \
    src/htmlgen/element.h \
    src/htmlgen/encode.h \
    src/htmlgen/toc.h \
    src/htmlgen/lua_sandbox.h \
    src/htmlgen/page.h \
    src/htmlgen/module/image.h \
    src/htmlgen/module/module.h \
    src/htmlgen/module/template.h \
    src/netresponse/response_template.h \
    src/netresponse/response_relay.h \
    src/htmlgen/user_controls.h \
    src/httpio/http_header.h \
    src/soloutils/ustring.h \
    src/htmlgen/wikieditor.h

OTHER_FILES +=

RESOURCES += \
    resources.qrc
