#pragma once

#include "texturelib_global.h"

#include <TextureLib/Texture>
#include <TextureLib/TextureIOHandlerPlugin>
#include <TextureLib/TextureIOResult>

#include <QtCore/QMimeType>
#include <QtCore/QScopedPointer>
#include <QtCore/QString>

#include <Expected>
#include <ObserverPointer>

QT_BEGIN_NAMESPACE
class QIODevice;
QT_END_NAMESPACE

class TextureIOPrivate;
class TEXTURELIB_EXPORT TextureIO
{
    Q_GADGET
    Q_DISABLE_COPY(TextureIO)
    Q_DECLARE_PRIVATE(TextureIO)

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
    Q_PROPERTY(QIODevicePointer device READ device WRITE setDevice)
    Q_PROPERTY(QMimeType mimeType READ mimeType WRITE setMimeType)

public:
    using QIODevicePointer = ObserverPointer<QIODevice>;

    TextureIO();
    TextureIO(TextureIO &&other) noexcept;
    explicit TextureIO(const QString &fileName, const QMimeType &mimeType = QMimeType());
    explicit TextureIO(QIODevicePointer device, const QMimeType &mimeType = QMimeType());
    TextureIO(const QString &fileName, QStringView mimeType);
    TextureIO(QIODevicePointer device, QStringView mimeType);
    ~TextureIO();

    TextureIO &operator=(TextureIO &&other) noexcept;

    using ReadResult = Expected<Texture, TextureIOError>;
    using WriteResult = TextureIOResult;

    QString fileName() const;
    void setFileName(const QString &fileName);

    QIODevicePointer device() const;
    void setDevice(QIODevicePointer device);

    QMimeType mimeType() const;
    void setMimeType(const QMimeType &mimeType);
    void setMimeType(QStringView mimeType);

    ReadResult read();

    WriteResult write(const Texture &contents);

    using Capability = TextureIOHandlerPlugin::Capability;
    using Capabilities = TextureIOHandlerPlugin::Capabilities;
    static std::vector<QStringView> availableMimeTypes(Capabilities caps = Capability::ReadWrite);

    static gsl::span<const TextureFormat> readableFormats();
    static gsl::span<const TextureFormat> writableFormats();

    static QString pluginsDirPath();

private:
    QScopedPointer<TextureIOPrivate> d_ptr;
};
