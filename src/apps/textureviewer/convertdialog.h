#ifndef CONVERTDIALOG_H
#define CONVERTDIALOG_H

#include <QDialog>

#include <TextureLib/Texture>

namespace Ui {
class ConvertDialog;
}

class ConvertDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConvertDialog(QWidget *parent = nullptr);
    ~ConvertDialog() override;

    Texture::Alignment alignment() const;
    TextureFormat format() const;

private:
    std::unique_ptr<Ui::ConvertDialog> ui;
};

#endif // CONVERTDIALOG_H
