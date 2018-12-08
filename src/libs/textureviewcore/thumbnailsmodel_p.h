#ifndef THUMBNAILSMODEL_P_H
#define THUMBNAILSMODEL_P_H

#include "thumbnailsmodel.h"

#include <UtilsLib/TreeModelItem>

namespace TextureViewer {

class ThumbnailsModel::Item : public TreeModelItem<ThumbnailsModel::Item>
{
public:
    using Position = ThumbnailsModel::Position;

    QString text;
    QPair<int, int> index;
    Position position {-1, -1, -1, -1};
};

} // namespace TextureViewer

#endif // THUMBNAILSMODEL_P_H
