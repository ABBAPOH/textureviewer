/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Copyright (C) 2013 Ivan Komissarov.
** Contact: http://www.qt-project.org/legal
**
** This file is part of the DDS plugin in the Qt ImageFormats module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <TextureLib/TextureIO>

static bool verifyTexture(const Texture &texture, const QImage &second)
{
    QImage image = second.convertToFormat(QImage::Format_ARGB32);

    if (texture.width() != image.width())
        return false;

    if (texture.height() != image.height())
        return false;

    if (texture.bytes() != image.sizeInBytes())
        return false;

    return memcmp(texture.data().data(), image.bits(), image.sizeInBytes()) == 0;
}

class TestDds: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testRead_data();
    void testRead();
    void benchRead_data();
    void benchRead();
};

void TestDds::initTestCase()
{
    qApp->addLibraryPath(qApp->applicationDirPath() + TextureIO::pluginsDirPath());
    Q_INIT_RESOURCE(images);
    QLoggingCategory::setFilterRules(QStringLiteral("plugins.textureformats.ddshandler.debug=false"));
}

void TestDds::testRead_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");

    QTest::newRow("8") << QString("A8R8G8B8") << 64 << 64;
}

void TestDds::testRead()
{
    QFETCH(QString, fileName);
    QFETCH(int, width);
    QFETCH(int, height);

    const QString path = QStringLiteral(":/data/") + fileName + QStringLiteral(".dds");
    const QString sourcePath = QStringLiteral(":/data/") + fileName + QStringLiteral(".png");
    TextureIO reader(path);
//    QVERIFY(reader.canRead());
    const auto result = reader.read();
    QVERIFY2(result, qPrintable(toUserString(result.error())));
    QCOMPARE(result->width(), width);
    QCOMPARE(result->height(), height);
    QVERIFY(verifyTexture(*result, QImage(sourcePath)));
}

void TestDds::benchRead_data()
{
    QTest::addColumn<QString>("fileName");

    QTest::newRow("RGBA8") << QStringLiteral(":/dds/RGBA8_Unorm.dds");
    QTest::newRow("L8") << QStringLiteral(":/dds/L8_Unorm.dds");
    QTest::newRow("DXT1") << QStringLiteral(":/dds/Bc1Rgb_Unorm.dds");
    QTest::newRow("DXT5") << QStringLiteral(":/dds/Bc3_Unorm.dds");
}

void TestDds::benchRead()
{
    QFETCH(QString, fileName);

    // preload QMimeDatabase
    const auto mt = QMimeDatabase().mimeTypeForName(QStringLiteral("image/x-dds"));
    QVERIFY(mt.isValid());

    // preload file (qrc is compressed, so we will test zlib otherwise)
    QFile source(fileName);
    QVERIFY(source.open(QIODevice::ReadOnly));
    QTemporaryFile file;
    QVERIFY(file.open());
    file.write(source.readAll());
    file.close();

    QBENCHMARK {
        TextureIO reader(file.fileName(), QStringLiteral("image/x-dds"));
        auto result = reader.read();
        QVERIFY2(result, qPrintable(toUserString(result.error())));
        QVERIFY(!result->isNull());
    }
}

QTEST_MAIN(TestDds)
#include "test_dds.moc"
