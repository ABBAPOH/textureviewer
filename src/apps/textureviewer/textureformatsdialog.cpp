#include "textureformatsdialog.h"
#include "ui_textureformatsdialog.h"

#include <TextureViewCoreLib/TextureFormatsModel>
#include <QHeaderView>

namespace TextureViewer {

TextureFormatsDialog::TextureFormatsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextureFormatsDialog)
{
    ui->setupUi(this);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    ui->treeView->setModel(new TextureFormatsModel(this));
}

TextureFormatsDialog::~TextureFormatsDialog() = default;

} // namespace TextureViewer
