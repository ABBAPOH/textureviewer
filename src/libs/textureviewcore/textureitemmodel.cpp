#include "textureitemmodel.h"

namespace TextureViewer {

namespace {

QString getDisplayRole(TextureItemModel::Dimension dim, int row)
{
    switch (dim) {
    case TextureItemModel::Dimension::Face:
        return TextureItemModel::tr("Face %1").arg(row);
    case TextureItemModel::Dimension::Level:
        return TextureItemModel::tr("Level %1").arg(row);
    case TextureItemModel::Dimension::Layer:
        return TextureItemModel::tr("Layer %1").arg(row);
    }
    Q_UNREACHABLE();
}

} // namespace

TextureItemModel::TextureItemModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

TextureItemModel::TextureItemModel(Dimension dimension, QObject *parent)
    : QAbstractListModel(parent)
    , m_dimension(dimension)
{
}

void TextureItemModel::setDocument(TextureItemModel::TextureDocumentPointer document)
{
    if (m_document == document)
        return;

    if (m_document) {
        disconnect(m_document.get(), nullptr, this, nullptr);
    }

    m_document = document;
    m_face = 0;
    m_level = 0;
    m_layer = 0;

    if (m_document) {
        connect(m_document.get(), &TextureDocument::textureChanged,
                this, &TextureItemModel::updateItems);
    }

    updateItems();

    emit documentChanged(m_document);
    emit faceChanged(m_face);
    emit levelChanged(m_level);
    emit layerChanged(m_layer);
}

void TextureItemModel::setDimension(Dimension dimension)
{
    if (m_dimension == dimension)
        return;
    m_dimension = dimension;
    updateItems();
    emit dimensionChanged(m_dimension);
}

void TextureItemModel::setFace(int face)
{
    if (m_face == face)
        return;
    m_face = face;
    updateItems();
    emit faceChanged(m_face);
}

void TextureItemModel::setLevel(int level)
{
    if (m_level == level)
        return;
    m_level = level;
    updateItems();
    emit levelChanged(m_level);
}

void TextureItemModel::setLayer(int layer)
{
    if (m_layer == layer)
        return;
    m_layer = layer;
    updateItems();
    emit layerChanged(m_layer);
}

QVariant TextureItemModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(
            index,
            QAbstractItemModel::CheckIndexOption::IndexIsValid
                    | QAbstractItemModel::CheckIndexOption::ParentIsInvalid)) {
        qWarning() << "Incorrect index passed to TextureItemModel::data";
        return QVariant();
    }

    const auto pos = index.row();
    if (role == Qt::DisplayRole) {
        return getDisplayRole(m_dimension, pos);
    } else if (role == Qt::DecorationRole) {
        return m_items[size_t(pos)]->thumbnail;
    }

    return QVariant();
}

int TextureItemModel::rowCount(const QModelIndex &parent) const
{
    if (!checkIndex(parent)) {
        qWarning() << "Incorrect index passed to TextureItemModel::rowCount";
        return 0;
    }
    return int(m_items.size());
}

int TextureItemModel::columnCount(const QModelIndex &parent) const
{
    if (!checkIndex(parent)) {
        qWarning() << "Incorrect index passed to TextureItemModel::columnCount";
        return 0;
    }
    return 1;
}

void TextureItemModel::updateItems()
{
    beginResetModel();
    m_items.clear();
    if (m_document) {
        switch (m_dimension) {
        case Dimension::Face:
            m_items.reserve(size_t(m_document->faces()));
            for (int i = 0; i < m_document->faces(); ++i) {
                m_items.push_back(m_document->item(i, m_level, m_layer));
            }
            break;
        case Dimension::Level:
            m_items.reserve(size_t(m_document->levels()));
            for (int i = 0; i < m_document->levels(); ++i) {
                m_items.push_back(m_document->item(m_face, i, m_layer));
            }
            break;
        case Dimension::Layer:
            m_items.reserve(size_t(m_document->layers()));
            for (int i = 0; i < m_document->layers(); ++i) {
                m_items.push_back(m_document->item(m_face, m_level, i));
            }
            break;
        }
    }
    endResetModel();
}

} // namespace TextureViewer
