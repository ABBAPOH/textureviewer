#include "texturelevelsmodel.h"

namespace TextureViewer {

TextureLevelsModel::TextureLevelsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

TextureLevelsModel::TextureLevelsModel(Qt::Orientation orientation, QObject *parent)
    : QAbstractListModel(parent)
    , m_orientation(orientation)
{
}

void TextureLevelsModel::setDocument(TextureLevelsModel::TextureDocumentPointer document)
{
    if (m_document == document)
        return;

    if (m_document) {
        disconnect(m_document.get(), nullptr, this, nullptr);
    }

    m_document = document;
    m_face = 0;

    if (m_document) {
        connect(m_document.get(), &TextureDocument::textureChanged, this, &TextureLevelsModel::updateItems);
    }

    updateItems();
    emit documentChanged(m_document);
    emit faceChanged(m_face);
}

void TextureLevelsModel::setFace(int face)
{
    if (m_face == face)
        return;
    m_face = face;
    updateItems();
    emit faceChanged(m_face);
}

void TextureLevelsModel::setLayer(int layer)
{
    if (m_layer == layer)
        return;
    m_layer = layer;
    updateItems();
    emit layerChanged(m_layer);
}

void TextureLevelsModel::setOrientation(Qt::Orientation orientation)
{
    if (m_orientation == orientation)
        return;
    beginResetModel();
    m_orientation = orientation;
    endResetModel();
    emit orientationChanged(m_orientation);
}

QVariant TextureLevelsModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(
            index,
            QAbstractItemModel::CheckIndexOption::IndexIsValid
                    | QAbstractItemModel::CheckIndexOption::ParentIsInvalid)) {
        qWarning() << "Incorrect index passed to TextureLevelsModel::data";
        return QVariant();
    }

    const auto pos = (m_orientation == Qt::Vertical) ? index.row() : index.column();
    if (role == Qt::DisplayRole) {
        return tr("Level %1").arg(pos);
    } else if (role == Qt::DecorationRole) {
        return m_items[pos]->thumbnail;
    }

    return QVariant();
}

int TextureLevelsModel::rowCount(const QModelIndex &parent) const
{
    if (!checkIndex(parent)) {
        qWarning() << "Incorrect index passed to TextureLevelsModel::rowCount";
        return 0;
    }
    return (m_orientation == Qt::Vertical) ? int(m_items.size()) : 1;
}

int TextureLevelsModel::columnCount(const QModelIndex &parent) const
{
    if (!checkIndex(parent)) {
        qWarning() << "Incorrect index passed to TextureLevelsModel::columnCount";
        return 0;
    }
    return (m_orientation == Qt::Horizontal) ? int(m_items.size()) : 1;
}

void TextureLevelsModel::updateItems()
{
    beginResetModel();
    m_items.clear();
    if (m_document) {
        for (int level = 0; level < m_document->levels(); ++level) {
            m_items.push_back(m_document->item(m_face, level, m_layer));
        }
    }
    endResetModel();
}

} // namespace TextureViewer
