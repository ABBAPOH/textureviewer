#include "textureformatsmodel.h"

#include <TextureLib/TextureIO>

#include <set>

TextureFormatsModel::TextureFormatsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    const std::set<TextureFormat> readableFormats = {
        TextureIO::readableFormats().begin(), TextureIO::readableFormats().end()
    };
    const std::set<TextureFormat> writableFormats = {
        TextureIO::writableFormats().begin(), TextureIO::writableFormats().end()
    };

    for (auto format = TextureFormat(qsizetype(TextureFormat::Invalid) + 1);
         format != TextureFormat::FormatsCount;
         format = TextureFormat(qsizetype(format) + 1)) {
        FormatInfo data;
        data.format = format;
        if (readableFormats.count(format))
            data.readable = true;
        if (writableFormats.count(format))
            data.readable = true;
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
            return toQString(m_data[row].format);
        } else if (column == int(Column::Readable)) {
            return m_data[row].readable;
        } else if (column == int(Column::Writable)) {
            return m_data[row].writable;
        }
    }

    return {};
}
