#include "texturedocument.h"

namespace TextureViewer {

class TextureDocumentPrivate
{
public:
    using Item = TextureDocument::Item;

    Texture texture;
    std::vector<std::unique_ptr<Item>> items;
};

/*!
  \class TextureViewer::TextureDocument
  \brief A document containing a Texture.
*/

/*!
  Constructs a TextureDocument object with the given \a parent.
*/
TextureDocument::TextureDocument(QObject *parent)
    : AbstractDocument(parent)
    , d_ptr(new TextureDocumentPrivate())
{
}

/*!
  Destroys the TextureDocument object.
*/
TextureDocument::~TextureDocument() = default;

/*!
  \property Texture TextureDocument::texture
  \brief This property holds an assigned Texture instance.

  Default value is null texture.
*/

Texture TextureDocument::texture() const
{
    Q_D(const TextureDocument);
    return d->texture;
}

void TextureDocument::setTexture(const Texture &texture)
{
    Q_D(TextureDocument);
    if (d->texture == texture)
        return;
    d->items.clear();
    d->texture = texture;
    const auto size = d->texture.faces() * d->texture.layers() * d->texture.levels();
    d->items.reserve(size);
    for (int level = 0; level < d->texture.levels(); ++level) {
        for (int layer = 0; layer < d->texture.layers(); ++layer) {
            for (int face = 0; face < d->texture.faces(); ++face) {
                auto item = std::make_unique<Item>();
                item->level = level;
                item->layer = layer;
                item->face = face;
                auto slice = Texture(
                        d->texture.format(),
                        {d->texture.width(level), d->texture.height(level)},
                        {1, 1},
                        d->texture.alignment());
                if (slice.isNull()) {
                    qWarning() << "Can't create slice";
                    d->items.clear();
                    break;
                }
                const auto image = d->texture.imageData({Texture::Side(face), level, layer});
                Q_ASSERT(image.size() == slice.imageData({}).size());
                memcpy(slice.imageData({}).data(), image.data(), image.size());
                item->texture = slice;
                item->thumbnail = slice.toImage();
                d->items.push_back(std::move(item));
            }
        }
    }

    emit textureChanged(texture);
    emit widthChanged(width());
    emit heigthChanged(heigth());
    emit depthChanged(depth());
    emit levelsChanged(levels());
    emit layersChanged(layers());
    emit facesChanged(faces());
}

TextureFormat TextureDocument::format() const
{
    Q_D(const TextureDocument);
    return d->texture.format();
}

Texture::Alignment TextureDocument::alignment() const
{
    Q_D(const TextureDocument);
    return d->texture.alignment();
}

int TextureDocument::width() const
{
    Q_D(const TextureDocument);
    return !d->texture.isNull() ? d->texture.width() : 0;
}

int TextureDocument::heigth() const
{
    Q_D(const TextureDocument);
    return !d->texture.isNull() ? d->texture.height() : 0;
}

int TextureDocument::depth() const
{
    Q_D(const TextureDocument);
    return !d->texture.isNull() ? d->texture.depth() : 0;
}

int TextureDocument::levels() const
{
    Q_D(const TextureDocument);
    return !d->texture.isNull() ? d->texture.levels() : 0;
}

int TextureDocument::layers() const
{
    Q_D(const TextureDocument);
    return !d->texture.isNull() ? d->texture.layers() : 0;
}

int TextureDocument::faces() const
{
    Q_D(const TextureDocument);
    return !d->texture.isNull() ? d->texture.faces() : 0;
}

auto TextureDocument::item(int face, int level, int layer) const -> ItemPointer
{
    Q_D(const TextureDocument);

    if (d->texture.isNull())
        return {};

    const auto index = d->texture.faces() * (d->texture.layers() * level + layer) + face;
    return ItemPointer(d->items.at(index).get());
}

} // namespace TextureViewer
