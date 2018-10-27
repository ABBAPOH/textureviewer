#ifndef TEXTUREFORMATSMODEL_H
#define TEXTUREFORMATSMODEL_H

#include "textureviewcore_global.h"

#include <TextureLib/TextureFormat>

#include <QtCore/QAbstractTableModel>

class TextureFormatInfo;

class TEXTUREVIEWCORE_EXPORT TextureFormatsModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_DISABLE_COPY(TextureFormatsModel)
public:
    enum class Column
    {
        Format,
        Readable,
        Writable,
        Type,
        Size,
        ColumnCount
    };

    explicit TextureFormatsModel(QObject *parent = nullptr);
    TextureFormatsModel(TextureFormatsModel &&) = delete;
    ~TextureFormatsModel() override = default;
    TextureFormatsModel &operator =(TextureFormatsModel &&) = delete;

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    struct FormatInfo
    {
        const TextureFormatInfo &info;
        bool readable {false};
        bool writable {false};
    };

    std::vector<FormatInfo> m_data;
};

#endif // TEXTUREFORMATSMODEL_H
