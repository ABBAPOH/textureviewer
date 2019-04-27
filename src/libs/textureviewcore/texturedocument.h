#ifndef TEXTUREDOCUMENT_H
#define TEXTUREDOCUMENT_H

#include "textureviewcore_global.h"

#include <TextureLib/Texture>

#include <UtilsLib/AbstractDocument>

#include <ObserverPointer>

#include <memory>

namespace TextureViewer {

class TextureDocumentPrivate;
class TEXTUREVIEWCORE_EXPORT TextureDocument : public AbstractDocument
{
    Q_OBJECT
    Q_DISABLE_COPY(TextureDocument)
    Q_DECLARE_PRIVATE(TextureDocument)
    Q_PROPERTY(Texture texture READ texture WRITE setTexture NOTIFY textureChanged)

    Q_PROPERTY(size_type width READ width NOTIFY widthChanged)
    Q_PROPERTY(size_type heigth READ heigth NOTIFY heigthChanged)
    Q_PROPERTY(size_type depth READ depth NOTIFY depthChanged)

    Q_PROPERTY(size_type levels READ levels NOTIFY levelsChanged)
    Q_PROPERTY(size_type layers READ layers NOTIFY layersChanged)
    Q_PROPERTY(size_type faces READ faces NOTIFY facesChanged)

    Q_PROPERTY(TextureFormat format READ format NOTIFY formatChanged)
    Q_PROPERTY(Texture::Alignment alignment READ alignment NOTIFY alignmentChanged)

public:
    using size_type = Texture::size_type;
    class Item;
    using ItemPointer = ObserverPointer<Item>;

    explicit TextureDocument(QObject *parent = nullptr);
    TextureDocument(TextureDocument &&) = delete;
    ~TextureDocument() override;
    TextureDocument &operator=(TextureDocument &&) = delete;

    Texture texture() const;
    void setTexture(const Texture &texture);

    TextureFormat format() const;
    Q_SIGNAL void formatChanged(TextureFormat format);

    Texture::Alignment alignment() const;
    Q_SIGNAL void alignmentChanged(Texture::Alignment);

    size_type width() const;
    Q_SIGNAL void widthChanged(size_type width);

    size_type heigth() const;
    Q_SIGNAL void heigthChanged(size_type heigth);

    size_type depth() const;
    Q_SIGNAL void depthChanged(size_type depth);

    size_type levels() const;
    size_type layers() const;
    size_type faces() const;

    ItemPointer item(size_type face, size_type level, size_type layer) const;

    bool convert(TextureFormat format, Texture::Alignment alignment);

signals:
    void textureChanged(const Texture &texture);

    void levelsChanged(size_type levels);
    void layersChanged(size_type layers);
    void facesChanged(size_type faces);

    // AbstractDocument interface
protected:
    void doOpen(const QUrl &url) override;
    void doSave(const QUrl &url) override;
    void doClear() override;
    void doCancel() override;
    bool doWaitForOpened() override;
    bool doWaitForSaved() override;

private:
    QScopedPointer<TextureDocumentPrivate> d_ptr;
};

class TextureDocument::Item
{
public:
    using size_type = Texture::size_type;
    size_type level {0};
    size_type layer {0};
    size_type face {0};
    Texture texture;
    QImage thumbnail;
};

} // namespace TextureViewer

Q_DECLARE_LOGGING_CATEGORY(texturedocument)

#endif // TEXTUREDOCUMENT_H
