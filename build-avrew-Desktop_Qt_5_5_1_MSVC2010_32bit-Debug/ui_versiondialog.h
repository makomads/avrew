/********************************************************************************
** Form generated from reading UI file 'versiondialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VERSIONDIALOG_H
#define UI_VERSIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_VersionDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *txtVersion;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *VersionDialog)
    {
        if (VersionDialog->objectName().isEmpty())
            VersionDialog->setObjectName(QStringLiteral("VersionDialog"));
        VersionDialog->resize(400, 300);
        verticalLayout = new QVBoxLayout(VersionDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        txtVersion = new QLabel(VersionDialog);
        txtVersion->setObjectName(QStringLiteral("txtVersion"));

        verticalLayout->addWidget(txtVersion);

        buttonBox = new QDialogButtonBox(VersionDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(VersionDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), VersionDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), VersionDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(VersionDialog);
    } // setupUi

    void retranslateUi(QDialog *VersionDialog)
    {
        VersionDialog->setWindowTitle(QApplication::translate("VersionDialog", "Dialog", 0));
        txtVersion->setText(QApplication::translate("VersionDialog", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class VersionDialog: public Ui_VersionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VERSIONDIALOG_H
