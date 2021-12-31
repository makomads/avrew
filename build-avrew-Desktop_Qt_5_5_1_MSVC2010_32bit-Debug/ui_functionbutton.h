/********************************************************************************
** Form generated from reading UI file 'functionbutton.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FUNCTIONBUTTON_H
#define UI_FUNCTIONBUTTON_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FunctionButton
{
public:
    QGridLayout *gridLayout;
    QPushButton *button;
    QLabel *label;

    void setupUi(QWidget *FunctionButton)
    {
        if (FunctionButton->objectName().isEmpty())
            FunctionButton->setObjectName(QStringLiteral("FunctionButton"));
        FunctionButton->resize(400, 300);
        gridLayout = new QGridLayout(FunctionButton);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        button = new QPushButton(FunctionButton);
        button->setObjectName(QStringLiteral("button"));

        gridLayout->addWidget(button, 0, 0, 1, 1);

        label = new QLabel(FunctionButton);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);


        retranslateUi(FunctionButton);

        QMetaObject::connectSlotsByName(FunctionButton);
    } // setupUi

    void retranslateUi(QWidget *FunctionButton)
    {
        FunctionButton->setWindowTitle(QApplication::translate("FunctionButton", "Form", 0));
        button->setText(QApplication::translate("FunctionButton", "b", 0));
        label->setText(QApplication::translate("FunctionButton", "txt", 0));
    } // retranslateUi

};

namespace Ui {
    class FunctionButton: public Ui_FunctionButton {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FUNCTIONBUTTON_H
