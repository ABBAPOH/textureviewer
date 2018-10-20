#ifndef THUMBNAILSMODEL_P_H
#define THUMBNAILSMODEL_P_H

#include "thumbnailsmodel.h"

namespace TextureViewer {

class ThumbnailsModel::Item
{
public:
    using ItemPointer = ObserverPointer<Item>;
    using ItemOwnedPointer = std::unique_ptr<Item>;

    Item() = default;
    Item(const Item &) = delete;
    Item(Item &&) = default;
    ~Item() = default;

    inline ItemPointer child(int row) const { return ItemPointer(m_children[size_t(row)].get()); }
    inline int childCount() const { return int(m_children.size()); }
    inline std::vector<ItemOwnedPointer> &children() { return m_children; }
    inline const std::vector<ItemOwnedPointer> &children() const { return m_children; }
    inline void append(ItemOwnedPointer item) { insert(childCount(), std::move(item)); }
    inline void insert(int row, ItemOwnedPointer item)
    {
        Q_ASSERT(row >= 0 && row <= childCount());
        item->m_parent = ItemPointer(this);
        m_children.insert(m_children.begin() + row, std::move(item));
    }
    inline ItemPointer createItem(int row = -1)
    {
        insert(row == - 1 ? childCount() : row, std::make_unique<Item>());
        return ItemPointer(m_children.back().get());
    }
    inline void remove(ItemPointer item)
    {
        const auto pred = [item](const ItemOwnedPointer &p) { return item.get() == p.get(); };
        const auto it = std::find_if(m_children.begin(), m_children.end(), pred);
        if (it != m_children.end())
            m_children.erase(it);
    }
    inline ItemPointer parent() const { return m_parent; }
    inline int row() const
    {
        if (!m_parent)
            return 0;
        const auto pred = [this](const ItemOwnedPointer &p) { return this == p.get(); };
        const auto it = std::find_if(m_parent->m_children.begin(), m_parent->m_children.end(), pred);
        return int(it - m_parent->m_children.begin());
    }

private:
    ItemPointer m_parent;
    std::vector<ItemOwnedPointer> m_children;

public:
    QString text;
    QPair<int, int> index;
};

} // namespace TextureViewer

#endif // THUMBNAILSMODEL_P_H
