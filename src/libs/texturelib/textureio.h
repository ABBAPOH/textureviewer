#pragma once

#include "texturelib_global.h"

#include <TextureLib/Texture>
#include <QtCore/QString>
#include <QtCore/QScopedPointer>
#include <QtCore/QMimeType>

class QIODevice;
class TextureIOResult;

class TextureIOPrivate;
class TEXTURELIB_EXPORT TextureIO
{
    Q_DISABLE_COPY(TextureIO)
    Q_DECLARE_PRIVATE(TextureIO)
public:

    TextureIO();
    explicit TextureIO(const QString &fileName, const QMimeType &mimeType = QMimeType());
    explicit TextureIO(QIODevice *device, const QMimeType &mimeType = QMimeType());
    TextureIO(const QString &fileName, const QString &mimeType);
    TextureIO(QIODevice *device, const QString &mimeType);
    ~TextureIO();

    QString fileName() const;
    void setFileName(const QString &fileName);

    QIODevice *device() const;
    void setDevice(QIODevice *device);

    QMimeType mimeType() const;
    void setMimeType(const QMimeType &mimeType);
    void setMimeType(const QString &mimeType);

    std::pair<TextureIOResult, Texture> read();

    TextureIOResult write(const Texture &contents);

    static QString pluginsDirPath();

private:
    QScopedPointer<TextureIOPrivate> d_ptr;
};
