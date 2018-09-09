#include "texturelayersmodel.h"

namespace TextureViewer {

TextureLayersModel::TextureLayersModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

TextureLayersModel::TextureLayersModel(Qt::Orientation orientation, QObject *parent)
    : QAbstractListModel(parent)
    , m_orientation(orientation)
{
}

void TextureLayersModel::setDocument(TextureLayersModel::TextureDocumentPointer document)
{
    if (m_document == document)
        return;

    if (m_document) {
        disconnect(m_document.get(), nullptr, this, nullptr);
    }

    m_document = document;
    m_face = 0;

    if (m_document) {
        connect(m_document.get(), &TextureDocument::textureChanged, this, &TextureLayersModel::updateItems);
    }

    updateItems();
    emit documentChanged(m_document);
    emit faceChanged(m_face);
}

void TextureLayersModel::setFace(int face)
{
    if (m_face == face)
        return;
    m_face = face;
    updateItems();
    emit faceChanged(m_face);
}

void TextureLayersModel::setOrientation(Qt::Orientation orientation)
{
    if (m_orientation == orientation)
        return;
    beginResetModel();
    m_orientation = orientation;
    endResetModel();
    emit orientationChanged(m_orientation);
}

QVariant TextureLayersModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(
            index,
            QAbstractItemModel::CheckIndexOption::IndexIsValid
                    | QAbstractItemModel::CheckIndexOption::ParentIsInvalid)) {
        qWarning() << "Incorrect index passed to TextureLayersModel::data";
        return QVariant();
    }

    const auto pos = (m_orientation == Qt::Vertical) ? index.row() : index.column();
    if (role == Qt::DisplayRole) {
        return tr("Layer %1").arg(pos);
    } else if (role == Qt::DecorationRole) {
        return m_items[pos]->thumbnail;
    }

    return QVariant();
}

int TextureLayersModel::rowCount(const QModelIndex &parent) const
{
    if (!checkIndex(parent)) {
        qWarning() << "Incorrect index passed to TextureLayersModel::rowCount";
        return 0;
    }
    return (m_orientation == Qt::Vertical) ? int(m_items.size()) : 1;
}

int TextureLayersModel::columnCount(const QModelIndex &parent) const
{
    if (!checkIndex(parent)) {
        qWarning() << "Incorrect index passed to TextureLayersModel::columnCount";
        return 0;
    }
    return (m_orientation == Qt::Horizontal) ? int(m_items.size()) : 1;
}

void TextureLayersModel::updateItems()
{
    beginResetModel();
    m_items.clear();
    if (m_document) {
        for (int layer = 0; layer < m_document->layers(); ++layer) {
            m_items.push_back(m_document->item(m_face, layer, 0));
        }
    }
    endResetModel();
}

} // namespace TextureViewer
