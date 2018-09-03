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
    Q_DECLARE_PRIVATE(TextureDocument)
    Q_PROPERTY(Texture texture READ texture WRITE setTexture NOTIFY textureChanged)

    Q_PROPERTY(int levels READ levels NOTIFY levelsChanged)
    Q_PROPERTY(int layers READ layers NOTIFY layersChanged)
    Q_PROPERTY(int faces READ faces NOTIFY facesChanged)

public:
    class Item;
    using ItemPointer = ObserverPointer<Item>;

    explicit TextureDocument(QObject *parent = nullptr);
    ~TextureDocument() override;

    Texture texture() const;
    void setTexture(const Texture &texture);

    int levels() const;
    int layers() const;
    int faces() const;

    ItemPointer item(int face, int level, int layer) const;

signals:
    void textureChanged(const Texture &texture);

    void levelsChanged(int levels);
    void layersChanged(int layers);
    void facesChanged(int faces);

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
};

} // namespace TextureViewer

#endif // TEXTUREDOCUMENT_H
