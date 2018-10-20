#ifndef THUMBNAILSMODEL_H
#define THUMBNAILSMODEL_H

#include "textureviewcore_global.h"

#include <TextureViewCoreLib/TextureDocument>

#include <QtCore/QAbstractItemModel>

#include <memory>

namespace TextureViewer {

class TEXTUREVIEWCORE_EXPORT ThumbnailsModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_DISABLE_COPY(ThumbnailsModel)
public:
    using TextureDocumentPointer = ObserverPointer<TextureDocument>;

    enum Role { IndexRole = Qt::UserRole };

    explicit ThumbnailsModel(QObject *parent = nullptr);
    ThumbnailsModel(ThumbnailsModel &&) = delete;
    ~ThumbnailsModel() override;

    ThumbnailsModel &operator =(ThumbnailsModel &&) = delete;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    TextureDocumentPointer document() const noexcept { return m_document; }
    void setDocument(const TextureDocumentPointer &document);

private:
    class Item;
    using ItemPointer = ObserverPointer<Item>;

    QModelIndex index(Item *item) const;
    ItemPointer item(const QModelIndex &index) const;
    void rebuildModel();
    void rebuildModel(ItemPointer parent);
    void rebuildModel(ItemPointer parent, int level);
    void rebuildModel(ItemPointer parent, int layer, int level);

private:
    std::unique_ptr<Item> m_rootItem;
    TextureDocumentPointer m_document { nullptr };
};

} // namespace TextureViewer

#endif // THUMBNAILSMODEL_H
