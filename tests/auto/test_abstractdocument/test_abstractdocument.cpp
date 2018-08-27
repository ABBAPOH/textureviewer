#include <QtTest>

#include <UtilsLib/AbstractDocument>
#include <QMimeDatabase>

class SimpleDocument : public AbstractDocument
{
public:
    explicit SimpleDocument(QObject *parent = nullptr) : AbstractDocument(parent) {}

    inline QString text() const { return _text; }
    void setText(const QString &text) { _text = text; }

    void open(const QUrl &url);
    void save(const QUrl &url);

private:
    QString _text;
};

void SimpleDocument::open(const QUrl &url)
{
    QVERIFY(url.isLocalFile());
    QFile file(url.toLocalFile());
    QVERIFY(file.open(QIODevice::ReadOnly));
    const auto data = file.readAll();
    _text = QString::fromUtf8(data);
}

void SimpleDocument::save(const QUrl &url)
{
    QVERIFY(url.isLocalFile());
    QFile file(url.toLocalFile());
    QVERIFY(file.open(QIODevice::WriteOnly));

    auto data = _text.toUtf8();
    QVERIFY(file.write(data) == data.length());
}

class TestDocument : public QObject
{
    Q_OBJECT
private slots:
    void defaultValues();
    void setters();
    void read();
    void write();
};

void TestDocument::defaultValues()
{
    SimpleDocument doc;
    QCOMPARE(doc.isModified(), false);
}

void TestDocument::setters()
{
    QFile file("file.txt");
    file.remove();
    file.open(QIODevice::WriteOnly);
    file.write("Hello, world!");
    file.close();

    SimpleDocument doc;

    doc.setModified(true);
    QCOMPARE(doc.isModified(), true);

    file.remove();
}

void TestDocument::read()
{
    QFile file("file.txt");
    file.remove();
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("Hello, world!");
    file.close();

    SimpleDocument doc;
    doc.open(QUrl::fromLocalFile("file.txt"));

    QCOMPARE(doc.text(), QStringLiteral("Hello, world!"));
    file.remove();
}

void TestDocument::write()
{
    SimpleDocument doc;
    doc.setText(QStringLiteral("Hello, world!"));
    doc.save(QUrl::fromLocalFile("file.txt"));

    QFile file("file.txt");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QCOMPARE(file.readAll(), QByteArray("Hello, world!"));
}

QTEST_APPLESS_MAIN(TestDocument)

#include "test_abstractdocument.moc"
