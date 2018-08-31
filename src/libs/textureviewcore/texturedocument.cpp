#include "texturedocument.h"

class TextureDocumentPrivate
{
public:
    using Item = TextureDocument::Item;

    Texture texture;
    std::vector<std::unique_ptr<Item>> items;
};

TextureDocument::TextureDocument(QObject *parent)
    : AbstractDocument(parent)
    , d_ptr(new TextureDocumentPrivate())
{
}

TextureDocument::~TextureDocument()
{
}

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
                d->items.push_back(std::move(item));
            }
        }
    }
    emit textureChanged(texture);
}

auto TextureDocument::item(int face, int level, int layer) const -> ItemPointer
{
    Q_D(const TextureDocument);

    if (d->texture.isNull())
        return ItemPointer();

    const auto index = d->texture.faces() * (d->texture.layers() * level + layer) + face;
    return ItemPointer(d->items.at(index).get());
}
