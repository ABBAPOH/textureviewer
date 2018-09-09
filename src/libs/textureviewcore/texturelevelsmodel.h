#ifndef TEXTURELEVELSMODEL_H
#define TEXTURELEVELSMODEL_H

#include <QtCore/QAbstractListModel>

#include <TextureViewCoreLib/TextureDocument>

namespace TextureViewer {

class TextureLevelsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(TextureLevelsModel)

    Q_PROPERTY(TextureDocumentPointer document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(int face READ face WRITE setFace NOTIFY faceChanged)
    Q_PROPERTY(int layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

public:
    using TextureDocumentPointer = ObserverPointer<TextureDocument>;

    explicit TextureLevelsModel(QObject *parent = nullptr);
    explicit TextureLevelsModel(Qt::Orientation orientation, QObject *parent = nullptr);
    ~TextureLevelsModel() override = default;

    TextureDocumentPointer document() const noexcept { return m_document; }
    void setDocument(TextureDocumentPointer document);
    Q_SIGNAL void documentChanged(TextureDocumentPointer document);

    int face() const noexcept { return m_face; }
    void setFace(int face);
    Q_SIGNAL void faceChanged(int face);

    int layer() const noexcept { return m_layer; }
    void setLayer(int layer);
    Q_SIGNAL void layerChanged(int layer);

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
    int m_layer {0};
    Qt::Orientation m_orientation {Qt::Orientation::Vertical};
    std::vector<TextureDocument::ItemPointer> m_items;
};

} // namespace TextureViewer

#endif // TEXTURELEVELSMODEL_H
