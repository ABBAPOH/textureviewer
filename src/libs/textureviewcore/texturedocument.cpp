#include "texturedocument.h"

#include <TextureLib/TextureIO>
#include <QLabel>
#include <QPixmap>

#include <QtConcurrent/QtConcurrentRun>
#include <QtCore/QSysInfo>

namespace TextureViewer {

class TextureDocumentPrivate
{
public:
    using Item = TextureDocument::Item;
    using ReadWatcher = QFutureWatcher<TextureIO::ReadResult>;
    using WriteWatcher = QFutureWatcher<TextureIO::WriteResult>;

    Texture texture;
    Texture visibleTexture; // TODO (abbapoh): move to Control
    std::vector<std::unique_ptr<Item>> items;

    std::unique_ptr<QFutureWatcher<TextureIO::ReadResult>> readWatcher;
    std::unique_ptr<QFutureWatcher<TextureIO::WriteResult>> writeWatcher;
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
    Q_D(TextureDocument);
    d->readWatcher = std::make_unique<TextureDocumentPrivate::ReadWatcher>();

    const auto onOpenFinished = [this]()
    {
        Q_D(TextureDocument);
        const auto future = d->readWatcher->future();
        if (future.isCanceled())
            return;
        const auto result = future.result();
        if (result) {
            setTexture(*result);
            endOpen(true);
        } else {
            endOpen(false, toUserString(result.error()));
        }
    };
    connect(d->readWatcher.get(), &QFutureWatcherBase::finished, this, onOpenFinished);

    d->writeWatcher = std::make_unique<TextureDocumentPrivate::WriteWatcher>();

    const auto onSaveFinished = [this]()
    {
        Q_D(TextureDocument);
        const auto future = d->writeWatcher->future();
        if (future.isCanceled())
            return;
        const auto result = future.result();
        if (result)
            endSave(true);
        else
            endSave(false, toUserString(result.error()));
    };
    connect(d->writeWatcher.get(), &QFutureWatcherBase::finished, this, onSaveFinished);
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
    d->texture = Texture();

    Q_ASSERT_X(
            QSysInfo::ByteOrder == QSysInfo::LittleEndian,
            "setTexture",
            "Big endian is not supported");
    d->visibleTexture = texture.convert(TextureFormat::RGBA8_Unorm, Texture::Alignment::Word);
    if (d->visibleTexture.isNull()) {
        qWarning() << "Can't convert texture to RGBA8";
        return;
    }

    d->texture = texture;

    const auto size = d->visibleTexture.faces() * d->visibleTexture.layers() * d->visibleTexture.levels();
    d->items.reserve(size);
    for (int level = 0; level < d->visibleTexture.levels(); ++level) {
        for (int layer = 0; layer < d->visibleTexture.layers(); ++layer) {
            for (int face = 0; face < d->visibleTexture.faces(); ++face) {
                auto item = std::make_unique<Item>();
                item->level = level;
                item->layer = layer;
                item->face = face;
                auto image = Texture(
                        d->visibleTexture.format(),
                        {d->visibleTexture.width(level), d->visibleTexture.height(level)},
                        {1, 1},
                        d->visibleTexture.alignment());
                if (image.isNull()) {
                    qWarning() << "Can't create slice";
                    d->items.clear();
                    break;
                }
                const auto imageData = d->visibleTexture.imageData({Texture::Side(face), level, layer});
                Q_ASSERT(imageData.size() == image.imageData({}).size());
                memcpy(image.imageData({}).data(), imageData.data(), imageData.size());
                item->texture = image;
                item->thumbnail = image.toImage();
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

auto TextureDocument::width() const -> size_type
{
    Q_D(const TextureDocument);
    return !d->texture.isNull() ? d->texture.width() : 0;
}

auto TextureDocument::heigth() const -> size_type
{
    Q_D(const TextureDocument);
    return !d->texture.isNull() ? d->texture.height() : 0;
}

auto TextureDocument::depth() const -> size_type
{
    Q_D(const TextureDocument);
    return !d->texture.isNull() ? d->texture.depth() : 0;
}

auto TextureDocument::levels() const -> size_type
{
    Q_D(const TextureDocument);
    return !d->texture.isNull() ? d->texture.levels() : 0;
}

auto TextureDocument::layers() const -> size_type
{
    Q_D(const TextureDocument);
    return !d->texture.isNull() ? d->texture.layers() : 0;
}

auto TextureDocument::faces() const -> size_type
{
    Q_D(const TextureDocument);
    return !d->texture.isNull() ? d->texture.faces() : 0;
}

auto TextureDocument::item(size_type face, size_type level, size_type layer) const -> ItemPointer
{
    Q_D(const TextureDocument);

    if (d->texture.isNull())
        return {};

    const auto index = d->texture.faces() * (d->texture.layers() * level + layer) + face;
    return ItemPointer(d->items.at(index).get());
}

bool TextureDocument::convert(TextureFormat format, Texture::Alignment alignment)
{
    Q_D(TextureDocument);
    if (d->texture.isNull()) {
        qCWarning(texturedocument) << "Can't convert null texture";
        return false;
    }
    if (format == d->texture.format() && alignment == d->texture.alignment())
        return true; // nothing to do
    const auto converted = d->texture.convert(format, alignment);
    if (converted.isNull()) {
        qCWarning(texturedocument) << "Can't convert texture";
        return false;
    }
    setTexture(converted);
    return true;
}

void TextureDocument::doOpen(const QUrl &url)
{
    Q_D(TextureDocument);
    beginOpen();
    if (!url.isLocalFile()) {
        endOpen(false, tr("Can't open non-local files"));
        return;
    }

    const auto openFunc = [](QUrl url) -> TextureIO::ReadResult
    {
        const auto path = url.toLocalFile();
        TextureIO io(path);
        return io.read();
    };

    d->readWatcher->setFuture(QtConcurrent::run(openFunc, url));
}

void TextureDocument::doSave(const QUrl &url)
{
    Q_D(TextureDocument);
    beginSave();
    if (!url.isLocalFile()) {
        endSave(false, tr("Can't save to non-local files"));
        return;
    }

    const auto texture = d->texture;
    const auto saveFunc = [texture](QUrl url) -> TextureIO::WriteResult
    {
        const auto path = url.toLocalFile();
        TextureIO io(path);
        return io.write(texture);
    };

    d->writeWatcher->setFuture(QtConcurrent::run(saveFunc, url));
}

void TextureDocument::doClear()
{
    setTexture(Texture());
}

void TextureDocument::doCancel()
{
    Q_D(TextureDocument);
    if (state() == State::Opening) {
        d->readWatcher->future().cancel();
        d->readWatcher->setFuture(QFuture<TextureIO::ReadResult>());
        openFinished(false, tr("Canceled"));
    }
    if (state() == State::Saving) {
        d->writeWatcher->future().cancel();
        d->writeWatcher->setFuture(QFuture<TextureIO::WriteResult>());
        saveFinished(false, tr("Canceled"));
    }
}

bool TextureDocument::doWaitForOpened()
{
    if (state() != State::Opening)
        return false;
    Q_D(TextureDocument);
    auto future = d->readWatcher->future();
    future.waitForFinished();
    return bool(future.result());
}

bool TextureDocument::doWaitForSaved()
{
    if (state() != State::Saving)
        return false;
    Q_D(TextureDocument);
    auto future = d->writeWatcher->future();
    future.waitForFinished();
    return bool(future.result());
}

} // namespace TextureViewer

Q_LOGGING_CATEGORY(texturedocument, "textureviewcore.texturedocument");

