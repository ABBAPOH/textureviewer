#include "textureformatsmodel.h"

#include <TextureLib/TextureIO>

#include <set>

TextureFormatsModel::TextureFormatsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    const auto readableFormats = TextureIO::readableFormats();
    const auto writableFormats = TextureIO::writableFormats();

    const auto readableFormatsSet = std::set<TextureFormat>{
        readableFormats.begin(), readableFormats.end()
    };
    const auto writableFormatsSet = std::set<TextureFormat>{
        writableFormats.begin(), writableFormats.end()
    };

    m_data.reserve(size_t(TextureFormat::FormatsCount));
    for (const auto &info: TextureFormatInfo::allFormatInfos()) {
        if (info.format() == TextureFormat::Invalid)
            continue;
        FormatInfo data {
            info,
            bool(readableFormatsSet.count(info.format())),
            bool(writableFormatsSet.count(info.format()))
        };
        if (readableFormatsSet.count(info.format()))
            data.readable = true;
        if (writableFormatsSet.count(info.format()))
            data.writable = true;
        m_data.push_back(data);
    }
}

int TextureFormatsModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return int(m_data.size());
    return 0;
}

int TextureFormatsModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return int(Column::ColumnCount);
    return 0;
}

QVariant TextureFormatsModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(
            index,
            QAbstractItemModel::CheckIndexOption::IndexIsValid
                    | QAbstractItemModel::CheckIndexOption::ParentIsInvalid)) {
        qWarning() << "Incorrect index passed to TextureFormatsModel::data";
        return {};
    }

    const auto row = size_t(index.row());
    const auto column = index.column();
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (column == int(Column::Format)) {
            return toQString(m_data[row].info.format());
        } else if (column == int(Column::Readable)) {
            return m_data[row].readable;
        } else if (column == int(Column::Writable)) {
            return m_data[row].writable;
        } else if (column == int(Column::Type)) {
            return toQString(m_data[row].info.type());
        } else if (column == int(Column::Size)) {
            return m_data[row].info.size();
        }
    }

    return {};
}

QVariant TextureFormatsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            if (section == int(Column::Format))
                return tr("Texture Format");
            else if (section == int(Column::Readable))
                return tr("Readable");
            else if (section == int(Column::Writable))
                return tr("Writable");
            else if (section == int(Column::Type))
                return tr("Type");
            else if (section == int(Column::Size))
                return tr("Size");
        } else if (role == Qt::ToolTipRole) {
            if (section == int(Column::Format))
                return tr("The name of the texture format");
            else if (section == int(Column::Readable))
                return tr("True if at least one plugin can read the format");
            else if (section == int(Column::Writable))
                return tr("True if at least one plugin can write the format");
            else if (section == int(Column::Type))
                return tr("The channel type of the format");
            else if (section == int(Column::Size))
                return tr("Number of bytes per texel or block size for uncompressed/compressed "
                          "formats, respectively");
        }
    }

    return {};
}
