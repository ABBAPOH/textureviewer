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

    Q_PROPERTY(int width READ width NOTIFY widthChanged)
    Q_PROPERTY(int heigth READ heigth NOTIFY heigthChanged)
    Q_PROPERTY(int depth READ depth NOTIFY depthChanged)

    Q_PROPERTY(int levels READ levels NOTIFY levelsChanged)
    Q_PROPERTY(int layers READ layers NOTIFY layersChanged)
    Q_PROPERTY(int faces READ faces NOTIFY facesChanged)

    Q_PROPERTY(TextureFormat format READ format NOTIFY formatChanged)
    Q_PROPERTY(Texture::Alignment alignment READ alignment NOTIFY alignmentChanged)

public:
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

    int width() const;
    Q_SIGNAL void widthChanged(int width);

    int heigth() const;
    Q_SIGNAL void heigthChanged(int heigth);

    int depth() const;
    Q_SIGNAL void depthChanged(int depth);

    int levels() const;
    int layers() const;
    int faces() const;

    ItemPointer item(int face, int level, int layer) const;

    bool convert(TextureFormat format, Texture::Alignment alignment);

signals:
    void textureChanged(const Texture &texture);

    void levelsChanged(int levels);
    void layersChanged(int layers);
    void facesChanged(int faces);

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
    int level {0};
    int layer {0};
    int face {0};
    Texture texture;
    QImage thumbnail;
};

} // namespace TextureViewer

Q_DECLARE_LOGGING_CATEGORY(texturedocument)

#endif // TEXTUREDOCUMENT_H
