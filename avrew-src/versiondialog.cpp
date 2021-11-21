#include "versiondialog.h"
#include "ui_versiondialog.h"

VersionDialog::VersionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionDialog)
{
    ui->setupUi(this);
    ui->txtVersion->setOpenExternalLinks(true);
}

void VersionDialog::setText(const QString& text)
{
    ui->txtVersion->setText(text);
}

VersionDialog::~VersionDialog()
{
    delete ui;
}
