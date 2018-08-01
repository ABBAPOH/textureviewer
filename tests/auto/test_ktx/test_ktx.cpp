#include <TextureLib/TextureIO>

#include <QtTest/QtTest>

#include <QtCore/QMimeDatabase>

class TestKTX: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void benchRead_data();
    void benchRead();
};

void TestKTX::initTestCase()
{
    qApp->addLibraryPath(qApp->applicationDirPath() + TextureIO::pluginsDirPath());
    Q_INIT_RESOURCE(images);
    QLoggingCategory::setFilterRules(QStringLiteral("plugins.textureformats.ktxhandler.debug=false"));
}

void TestKTX::benchRead_data()
{
    QTest::addColumn<QString>("fileName");

    QTest::newRow("RGB8_ETC2") << QStringLiteral(":/ktx/RGB8_ETC2.ktx");
}

void TestKTX::benchRead()
{
    QFETCH(QString, fileName);

    // preload QMimeDatabase
    const auto mt = QMimeDatabase().mimeTypeForName(QStringLiteral("image/x-ktx"));
    QVERIFY(mt.isValid());

    // preload file (qrc is compressed, so we will test zlib otherwise)
    QFile source(fileName);
    QVERIFY(source.open(QIODevice::ReadOnly));
    QTemporaryFile file;
    QVERIFY(file.open());
    file.write(source.readAll());
    file.close();

    QBENCHMARK {
        TextureIO reader(file.fileName(), QStringLiteral("image/x-ktx"));
        auto result = reader.read();
        QVERIFY2(result, qPrintable(toUserString(result.error())));
        QVERIFY(!result->isNull());
    }
}

QTEST_MAIN(TestKTX)
#include "test_ktx.moc"
