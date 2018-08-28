#ifndef TEXTUREDOCUMENT_H
#define TEXTUREDOCUMENT_H

#include <TextureLib/Texture>

#include <UtilsLib/AbstractDocument>

#include <memory>

class TextureDocumentPrivate;
class TextureDocument : public AbstractDocument
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(TextureDocument)
    Q_PROPERTY(Texture texture READ texture WRITE setTexture NOTIFY textureChanged)
public:
    explicit TextureDocument(QObject *parent = nullptr);
    ~TextureDocument() override;

    Texture texture() const;
    void setTexture(const Texture &texture);

signals:
    void textureChanged(const Texture &texture);

private:
    QScopedPointer<TextureDocumentPrivate> d_ptr;
};

#endif // TEXTUREDOCUMENT_H
