#pragma once

#include "texturelib_global.h"

#include <TextureLib/Texture>
#include <TextureLib/TextureIOResult>
#include <QtCore/QString>
#include <QtCore/QScopedPointer>
#include <QtCore/QMimeType>

#include <Expected>
#include <ObserverPointer>

class QIODevice;

class TextureIOPrivate;
class TEXTURELIB_EXPORT TextureIO
{
    Q_DISABLE_COPY(TextureIO)
    Q_DECLARE_PRIVATE(TextureIO)
public:
    using QIODevicePointer = ObserverPointer<QIODevice>;

    TextureIO();
    explicit TextureIO(const QString &fileName, const QMimeType &mimeType = QMimeType());
    explicit TextureIO(QIODevicePointer device, const QMimeType &mimeType = QMimeType());
    TextureIO(const QString &fileName, const QString &mimeType);
    TextureIO(QIODevicePointer device, const QString &mimeType);
    ~TextureIO();

    using ReadResult = Expected<Texture, TextureIOError>;
    using WriteResult = TextureIOResult;

    QString fileName() const;
    void setFileName(const QString &fileName);

    QIODevicePointer device() const;
    void setDevice(QIODevicePointer device);

    QMimeType mimeType() const;
    void setMimeType(const QMimeType &mimeType);
    void setMimeType(const QString &mimeType);

    ReadResult read();

    WriteResult write(const Texture &contents);

    static QString pluginsDirPath();

private:
    QScopedPointer<TextureIOPrivate> d_ptr;
};
