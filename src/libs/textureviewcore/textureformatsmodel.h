#ifndef TEXTUREFORMATSMODEL_H
#define TEXTUREFORMATSMODEL_H

#include <TextureLib/TextureFormat>

#include <QtCore/QAbstractTableModel>

class TextureFormatsModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_DISABLE_COPY(TextureFormatsModel)
public:
    enum class Column
    {
        Format,
        Readable,
        Writable,
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
        TextureFormat format {TextureFormat::Invalid};
        bool readable {false};
        bool writable {false};
    };

    std::vector<FormatInfo> m_data;
};

#endif // TEXTUREFORMATSMODEL_H
