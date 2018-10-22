#include "convertdialog.h"
#include "ui_convertdialog.h"

#include <TextureLib/Texture>
#include <TextureLib/TextureIO>

#include <set>

ConvertDialog::ConvertDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConvertDialog)
{
    ui->setupUi(this);

    const auto supportedConvertions = Texture::supportedConvertions();
    const auto writableFormats = TextureIO::writableFormats();
    std::set<TextureFormat> supportedConvertionsSet =
            {supportedConvertions.begin(), supportedConvertions.end()};
    std::set<TextureFormat> writableFormatsSet =
            {writableFormats.begin(), writableFormats.end()};
    std::set<TextureFormat> formats;
    std::set_intersection(
            supportedConvertionsSet.begin(), supportedConvertionsSet.end(),
            writableFormatsSet.begin(), writableFormatsSet.end(),
            std::inserter(formats, formats.end()));
    for (const auto format: formats) {
        ui->formatComboBox->addItem(toQString(format), QVariant::fromValue(format));
    }
}

ConvertDialog::~ConvertDialog() = default;

Texture::Alignment ConvertDialog::alignment() const
{
    return ui->alignmentComboBox->currentIndex()
            ? Texture::Alignment::Word
            : Texture::Alignment::Byte;
}

TextureFormat ConvertDialog::format() const
{
    return ui->formatComboBox->currentData().value<TextureFormat>();
}
