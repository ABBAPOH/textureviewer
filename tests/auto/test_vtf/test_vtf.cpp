#include <TextureLib/TextureIO>

#include <QtTest/QtTest>

#include <QtCore/QMimeDatabase>
#include <QtCore/QTemporaryFile>

class TestVTF: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void benchRead_data();
    void benchRead();
};

void TestVTF::initTestCase()
{
    qApp->addLibraryPath(qApp->applicationDirPath() + TextureIO::pluginsDirPath());
    Q_INIT_RESOURCE(images);
    QLoggingCategory::setFilterRules(QStringLiteral("plugins.textureformats.vtfhandler.debug=false"));
}

void TestVTF::benchRead_data()
{
    QTest::addColumn<QString>("fileName");

    QTest::newRow("RGBA_8888") << QStringLiteral(":/vtf/RGBA_8888.vtf");
    QTest::newRow("L8") << QStringLiteral(":/vtf/L8.vtf");
    QTest::newRow("DXT1") << QStringLiteral(":/vtf/DXT1.vtf");
    QTest::newRow("DXT5") << QStringLiteral(":/vtf/DXT5.vtf");
}

void TestVTF::benchRead()
{
    QFETCH(QString, fileName);

    // preload QMimeDatabase
    const auto mt = QMimeDatabase().mimeTypeForName("image/x-vtf");
    QVERIFY(mt.isValid());

    QFile source(fileName);
    QVERIFY(source.open(QIODevice::ReadOnly));
    QTemporaryFile file;
    QVERIFY(file.open());
    file.write(source.readAll());
    file.close();

    QBENCHMARK {
        TextureIO reader(file.fileName(), QStringLiteral("image/x-vtf"));
        auto result = reader.read();
        QVERIFY2(result, qPrintable(toUserString(result.error())));
        QVERIFY(!result->isNull());
    }
}

QTEST_MAIN(TestVTF)
#include "test_vtf.moc"
