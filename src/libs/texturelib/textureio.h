#pragma once

#include "texturelib_global.h"

#include <TextureLib/Texture>
#include <TextureLib/TextureIOResult>
#include <QtCore/QString>
#include <QtCore/QScopedPointer>
#include <QtCore/QMimeType>

#include <tl/expected.h>

class QIODevice;

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

    template<class Value, class Error>
    using Expected = tl::expected<Value, Error>;

    QString fileName() const;
    void setFileName(const QString &fileName);

    QIODevice *device() const;
    void setDevice(QIODevice *device);

    QMimeType mimeType() const;
    void setMimeType(const QMimeType &mimeType);
    void setMimeType(const QString &mimeType);

    Expected<Texture, TextureIOError> read();

    TextureIOResult write(const Texture &contents);

    static QString pluginsDirPath();

private:
    QScopedPointer<TextureIOPrivate> d_ptr;
};
