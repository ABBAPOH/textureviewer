#include "textureformatsdialog.h"
#include "ui_textureformatsdialog.h"

#include <TextureViewCoreLib/TextureFormatsModel>

namespace TextureViewer {

TextureFormatsDialog::TextureFormatsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextureFormatsDialog)
{
    ui->setupUi(this);
    ui->treeView->setModel(new TextureFormatsModel(this));
}

TextureFormatsDialog::~TextureFormatsDialog() = default;

} // namespace TextureViewer
