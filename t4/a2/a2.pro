#-------------------------------------------------
#
# Project created by QtCreator 2018-06-10T23:30:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = a2
TEMPLATE = app

isEmpty(BOOST_LIB_PATH) {
    macx:BOOST_LIB_PATH = /opt/local/lib
}
isEmpty(BDB_LIB_PATH) {
    macx:BDB_LIB_PATH = /opt/local/lib/db48
}
isEmpty(BDB_LIB_SUFFIX) {
    macx:BDB_LIB_SUFFIX = -4.8
}
isEmpty(BOOST_LIB_SUFFIX) {
    macx:BOOST_LIB_SUFFIX = -mt
    windows:BOOST_LIB_SUFFIX = -mgw44-mt-s-1_50
}
isEmpty(BOOST_THREAD_LIB_SUFFIX) {
    BOOST_THREAD_LIB_SUFFIX = $$BOOST_LIB_SUFFIX
}

LIBS += $$join(BOOST_LIB_PATH,,-L,) $$join(BDB_LIB_PATH,,-L,) $$join(OPENSSL_LIB_PATH,,-L,) $$join(QRENCODE_LIB_PATH,,-L,)
LIBS += -lssl -lcrypto -ldb_cxx$$BDB_LIB_SUFFIX
LIBS += -lboost_system$$BOOST_LIB_SUFFIX -lboost_filesystem$$BOOST_LIB_SUFFIX -lboost_program_options$$BOOST_LIB_SUFFIX -lboost_thread$$BOOST_THREAD_LIB_SUFFIX


SOURCES +=\
        dialog.cpp \
    a2.cpp \
    net.cpp \
    netbase.cpp \
    protocol.cpp \
    addrman.cpp \
    irc.cpp \
    db.cpp \
    init.cpp \
    main.cpp \
    version.cpp \
    sync.cpp \
    util.cpp \
    script.cpp \
    alert.cpp \
    checkpoints.cpp \
    keystore.cpp \
    crypter.cpp \
    wallet.cpp \
    bitcoinrpc.cpp \
    json/json_spirit_reader.cpp \
    json/json_spirit_value.cpp \
    json/json_spirit_writer.cpp \
    walletdb.cpp \
    key.cpp \
    rpcblockchain.cpp

HEADERS  += dialog.h \
    net.h \
    mruset.h \
    netbase.h \
    protocol.h \
    addrman.h \
    irc.h \
    db.h \
    init.h \
    strlcpy.h \
    ui_interface.h \
    main.h \
    compat.h \
    serialize.h \
    allocators.h \
    version.h \
    clientversion.h \
    uint256.h \
    sync.h \
    util.h \
    bignum.h \
    script.h \
    alert.h \
    checkpoints.h \
    keystore.h \
    crypter.h \
    wallet.h \
    base58.h \
    bitcoinrpc.h \
    json/json_spirit.h \
    json/json_spirit_error_position.h \
    json/json_spirit_reader.h \
    json/json_spirit_reader_template.h \
    json/json_spirit_stream_reader.h \
    json/json_spirit_utils.h \
    json/json_spirit_value.h \
    json/json_spirit_writer.h \
    json/json_spirit_writer_template.h \
    walletdb.h \
    key.h

DISTFILES += \
    a2.pro.user
