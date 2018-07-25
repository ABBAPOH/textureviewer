#ifndef TEXTUREMODEL_H
#define TEXTUREMODEL_H

#include "texturelib_global.h"

#include <TextureLib/Texture>

#include <QtCore/QAbstractTableModel>

class TEXTURELIB_EXPORT TextureModel : public QAbstractTableModel
{
public:
    enum Column {
        ColumnName,
        ColumnValue,
        ColumnCount
    };

    enum Row {
        RowFormat,
        RowWidth,
        RowHeight,
        RowDepth,
        RowFaces,
        RowLevels,
        RowLayers,
        RowCount
    };

    explicit TextureModel(QObject *parent = nullptr);

    Texture texture() const;
    void setTexture(const Texture &texture);

public: // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    Texture m_texture;
};

#endif // TEXTUREMODEL_H
