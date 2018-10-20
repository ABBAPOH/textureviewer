#include "thumbnailsmodel.h"
#include "thumbnailsmodel_p.h"

namespace TextureViewer {

/*!
    \class ThumbnailsModel
*/

ThumbnailsModel::ThumbnailsModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_rootItem(std::make_unique<Item>())
{
}

ThumbnailsModel::~ThumbnailsModel() = default;

int ThumbnailsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int ThumbnailsModel::rowCount(const QModelIndex &parent) const
{
    return item(parent)->childCount();
}

QModelIndex ThumbnailsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    return createIndex(row, column, item(parent)->child(row).get());
}

QModelIndex ThumbnailsModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    const auto childItem = static_cast<Item*>(index.internalPointer());
    const auto parentItem = childItem->parent();

    if (parentItem.get() == m_rootItem.get())
        return {};

    return createIndex(parentItem->row(), 0, parentItem.get());
}

QVariant ThumbnailsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto item = static_cast<Item*>(index.internalPointer());
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return item->text;
    } else if (role == IndexRole) {
        return QVariant::fromValue(item->index);
    }

    return QVariant();
}

void ThumbnailsModel::setDocument(const TextureDocumentPointer &document)
{
    if (m_document == document)
        return;

    if (m_document) {
        disconnect(m_document.get(), nullptr, this, nullptr);
    }

    m_document = document;

    if (m_document) {
        connect(m_document.get(), &TextureDocument::textureChanged,
                this, static_cast<void (ThumbnailsModel::*)()>(&ThumbnailsModel::rebuildModel));
    }
    rebuildModel();
}

QModelIndex ThumbnailsModel::index(ThumbnailsModel::Item *item) const
{
    return createIndex(item->row(), 0, item);
}

auto ThumbnailsModel::item(const QModelIndex &index) const -> ItemPointer
{
    if (index.isValid())
        return ObserverPointer<Item>(static_cast<Item *>(index.internalPointer()));

    return ObserverPointer<Item>(m_rootItem.get());
}

void ThumbnailsModel::rebuildModel()
{
    beginResetModel();
    m_rootItem = std::make_unique<Item>();
    rebuildModel(ItemPointer(m_rootItem.get()));
    endResetModel();
}

void ThumbnailsModel::rebuildModel(ItemPointer parent)
{
    if (m_document->levels() <= 1) {
        rebuildModel(parent, 0);
    } else {
        for (int i = 0; i < m_document->levels(); ++i) {
            auto item = parent->createItem();
            item->text = tr("Mipmap %1").arg(i);
            rebuildModel(item, i);
        }
    }
}

void ThumbnailsModel::rebuildModel(ItemPointer parent, int level)
{
    if (m_document->layers() == 1) {
        rebuildModel(parent, 0, level);
    } else {
        for (int i = 0; i < m_document->layers(); i++) {
            auto item = parent->createItem();
            item->text = tr("Image %1").arg(i);
            rebuildModel(item, i, level);
        }
    }
}

void ThumbnailsModel::rebuildModel(ItemPointer parent, int layer, int level)
{
    auto item = parent;
    if (item.get() == m_rootItem.get()) {
        auto item = parent->createItem();
        item->text = tr("Image");
    }
    item->index = qMakePair(layer, level);
}

} // namespace TextureViewer
