#ifndef TEXTURELAYERSMODEL_H
#define TEXTURELAYERSMODEL_H

#include <QtCore/QAbstractListModel>

#include <TextureViewCoreLib/TextureDocument>

namespace TextureViewer {

class TextureLayersModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(TextureLayersModel)

    Q_PROPERTY(TextureDocumentPointer document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(int face READ face WRITE setFace NOTIFY faceChanged)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
public:
    using TextureDocumentPointer = ObserverPointer<TextureDocument>;

    explicit TextureLayersModel(QObject *parent = nullptr);
    explicit TextureLayersModel(Qt::Orientation orientation, QObject *parent = nullptr);
    ~TextureLayersModel() override = default;

    TextureDocumentPointer document() const noexcept { return m_document; }
    void setDocument(TextureDocumentPointer document);
    Q_SIGNAL void documentChanged(TextureDocumentPointer document);

    int face() const noexcept { return m_face; }
    void setFace(int face);
    Q_SIGNAL void faceChanged(int face);

    Qt::Orientation orientation() const noexcept { return m_orientation; }
    void setOrientation(Qt::Orientation orientation);
    Q_SIGNAL void orientationChanged(Qt::Orientation orientation);

public: // QAbstractItemModel interface
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

private:
    void updateItems();

private:
    TextureDocumentPointer m_document;
    int m_face {0};
    Qt::Orientation m_orientation {Qt::Orientation::Vertical};
    std::vector<TextureDocument::ItemPointer> m_items;
};

} // namespace TextureViewer

#endif // TEXTURELAYERSMODEL_H
