#include "texturemodel.h"

TextureModel::TextureModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

Texture TextureModel::texture() const
{
    return m_texture;
}

void TextureModel::setTexture(const Texture& contents)
{
    if (m_texture == contents)
        return;

    beginResetModel();
    m_texture = contents;
    endResetModel();
}

int TextureModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_texture.isNull() ? 0 : RowCount;
}

int TextureModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant TextureModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
            if (section == ColumnName)
                return tr("name");
            if (section == ColumnValue)
                return tr("value");
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant TextureModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole) {
        if (index.column() == ColumnName) {
            switch (index.row()) {
            case RowFormat: return tr("Format");
            case RowWidth: return tr("Width");
            case RowHeight: return tr("Height");
            case RowDepth: return tr("Depth");
            case RowFaces: return tr("Faces count");
            case RowLevels: return tr("Levels count");
            case RowLayers: return tr("Layers count");
            default:
                break;
            }
        } else if (index.column() == ColumnValue) {
            if (m_texture.isNull())
                return QString();
            switch (index.row()) {
            case RowFormat: return toQString(m_texture.format());
            case RowWidth: return m_texture.width();
            case RowHeight: return m_texture.height();
            case RowDepth: return m_texture.depth();
            case RowFaces: return m_texture.faces();
            case RowLevels: return m_texture.levels();
            case RowLayers: return m_texture.layers();
            default:
                break;
            }
        }
    }

    return QVariant();
}
