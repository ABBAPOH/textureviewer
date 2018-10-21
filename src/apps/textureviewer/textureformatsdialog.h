#ifndef TEXTUREFORMATSDIALOG_H
#define TEXTUREFORMATSDIALOG_H

#include <QtWidgets/QDialog>

#include <memory>

namespace Ui {
class TextureFormatsDialog;
}

namespace TextureViewer {

class TextureFormatsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextureFormatsDialog(QWidget *parent = nullptr);
    ~TextureFormatsDialog() override;

private:
    std::unique_ptr<Ui::TextureFormatsDialog> ui;
};

} // namespace TextureViewer

#endif // TEXTUREFORMATSDIALOG_H
