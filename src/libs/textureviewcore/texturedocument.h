#ifndef TEXTUREDOCUMENT_H
#define TEXTUREDOCUMENT_H

#include "textureviewcore_global.h"

#include <TextureLib/Texture>

#include <UtilsLib/AbstractDocument>

#include <ObserverPointer>

#include <memory>

class TextureDocumentPrivate;
class TEXTUREVIEWCORE_EXPORT TextureDocument : public AbstractDocument
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(TextureDocument)
    Q_PROPERTY(Texture texture READ texture WRITE setTexture NOTIFY textureChanged)
public:
    class Item;
    using ItemPointer = ObserverPointer<Item>;

    explicit TextureDocument(QObject *parent = nullptr);
    ~TextureDocument() override;

    Texture texture() const;
    void setTexture(const Texture &texture);

    ItemPointer item(int face, int level, int layer) const;

signals:
    void textureChanged(const Texture &texture);

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

#endif // TEXTUREDOCUMENT_H
