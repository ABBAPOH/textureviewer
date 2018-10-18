#ifndef TEXTUREITEMMODEL_H
#define TEXTUREITEMMODEL_H

#include "textureviewcore_global.h"

#include <QtCore/QAbstractListModel>

#include <TextureViewCoreLib/TextureDocument>

namespace TextureViewer {

class TEXTUREVIEWCORE_EXPORT TextureItemModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(TextureItemModel)

    Q_PROPERTY(TextureDocumentPointer document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(Dimension dimension READ dimension WRITE setDimension NOTIFY dimensionChanged)
    Q_PROPERTY(int face READ face WRITE setFace NOTIFY faceChanged)
    Q_PROPERTY(int level READ level WRITE setLevel NOTIFY levelChanged)
    Q_PROPERTY(int layer READ layer WRITE setLayer NOTIFY layerChanged)

public:
    using TextureDocumentPointer = ObserverPointer<TextureDocument>;
    enum class Dimension { Face, Level, Layer };

    explicit TextureItemModel(QObject *parent = nullptr);
    explicit TextureItemModel(Dimension dimension, QObject *parent = nullptr);

    TextureDocumentPointer document() const noexcept { return m_document; }
    void setDocument(TextureDocumentPointer document);
    Q_SIGNAL void documentChanged(TextureDocumentPointer document);

    Dimension dimension() const noexcept { return m_dimension; }
    void setDimension(Dimension dimension);
    Q_SIGNAL void dimensionChanged(Dimension dimension);

    int face() const noexcept { return m_face; }
    void setFace(int face);
    Q_SIGNAL void faceChanged(int face);

    int level() const noexcept { return m_level; }
    void setLevel(int level);
    Q_SIGNAL void levelChanged(int layer);

    int layer() const noexcept { return m_layer; }
    void setLayer(int layer);
    Q_SIGNAL void layerChanged(int layer);

public: // QAbstractItemModel interface
    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

private:
    void updateItems();

private:
    TextureDocumentPointer m_document;
    Dimension m_dimension {Dimension::Face};
    int m_face {0};
    int m_level {0};
    int m_layer {0};
    std::vector<TextureDocument::ItemPointer> m_items;
};

} // namespace TextureViewer

#endif // TEXTUREITEMMODEL_H
