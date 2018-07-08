#-------------------------------------------------
#
# Project created by QtCreator 2018-06-10T23:30:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT       += testlib

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
    rpcblockchain.cpp \
    qt/walletmodel.cpp \
    qt/test/test_main.cpp \
    qt/test/uritests.cpp \
    qt/aboutdialog.cpp \
    qt/addressbookpage.cpp \
    qt/addresstablemodel.cpp \
    qt/askpassphrasedialog.cpp \
    qt/bitcoin.cpp \
    qt/bitcoinaddressvalidator.cpp \
    qt/bitcoinamountfield.cpp \
    qt/bitcoingui.cpp \
    qt/bitcoinstrings.cpp \
    qt/bitcoinunits.cpp \
    qt/clientmodel.cpp \
    qt/csvmodelwriter.cpp \
    qt/editaddressdialog.cpp \
    qt/guiutil.cpp \
    qt/monitoreddatamapper.cpp \
    qt/notificator.cpp \
    qt/optionsdialog.cpp \
    qt/optionsmodel.cpp \
    qt/overviewpage.cpp \
    qt/qtipcserver.cpp \
    qt/qvalidatedlineedit.cpp \
    qt/qvaluecombobox.cpp \
    qt/rpcconsole.cpp \
    qt/sendcoinsdialog.cpp \
    qt/sendcoinsentry.cpp \
    qt/signverifymessagedialog.cpp \
    qt/transactiondesc.cpp \
    qt/transactiondescdialog.cpp \
    qt/transactionfilterproxy.cpp \
    qt/transactionrecord.cpp \
    qt/transactiontablemodel.cpp \
    qt/transactionview.cpp

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
    key.h \
    qt/walletmodel.h \
    qt/test/uritests.h \
    qt/aboutdialog.h \
    qt/addressbookpage.h \
    qt/addresstablemodel.h \
    qt/askpassphrasedialog.h \
    qt/bitcoinaddressvalidator.h \
    qt/bitcoinamountfield.h \
    qt/bitcoingui.h \
    qt/bitcoinunits.h \
    qt/clientmodel.h \
    qt/csvmodelwriter.h \
    qt/editaddressdialog.h \
    qt/guiconstants.h \
    qt/guiutil.h \
    qt/macdockiconhandler.h \
    qt/monitoreddatamapper.h \
    qt/notificator.h \
    qt/optionsdialog.h \
    qt/optionsmodel.h \
    qt/overviewpage.h \
    qt/qtipcserver.h \
    qt/qvalidatedlineedit.h \
    qt/qvaluecombobox.h \
    qt/rpcconsole.h \
    qt/sendcoinsdialog.h \
    qt/sendcoinsentry.h \
    qt/signverifymessagedialog.h \
    qt/transactiondesc.h \
    qt/transactiondescdialog.h \
    qt/transactionfilterproxy.h \
    qt/transactionrecord.h \
    qt/transactiontablemodel.h \
    qt/transactionview.h

DISTFILES += \
    a2.pro.user
