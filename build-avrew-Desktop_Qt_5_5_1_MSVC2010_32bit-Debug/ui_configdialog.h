/********************************************************************************
** Form generated from reading UI file 'configdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGDIALOG_H
#define UI_CONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfigDialog
{
public:
    QGridLayout *gridLayout_5;
    QTabWidget *tabWidget;
    QWidget *tab_3;
    QGridLayout *gridLayout_13;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_17;
    QGridLayout *gridLayout_16;
    QCheckBox *chkConnectOnBoot;
    QCheckBox *chkExecTargetAfterWriting;
    QGridLayout *gridLayout_4;
    QSpacerItem *verticalSpacer_3;
    QWidget *tab;
    QGridLayout *gridLayout_8;
    QGridLayout *gridLayout_7;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_9;
    QGridLayout *gridLayout;
    QComboBox *cmbBaudRate;
    QComboBox *cmbSynchMode;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label;
    QComboBox *cmbComPort;
    QSpacerItem *horizontalSpacer;
    QLabel *label_5;
    QLabel *label_7;
    QLineEdit *edtSPIDelay;
    QSpacerItem *verticalSpacer;
    QWidget *tab_2;
    QGridLayout *gridLayout_11;
    QGridLayout *gridLayout_10;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout_12;
    QLabel *label_16;
    QComboBox *cmbNewLine;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_18;
    QGridLayout *gridLayout_3;
    QLabel *label_9;
    QComboBox *cmbPB1Init;
    QComboBox *cmbPB2Init;
    QLineEdit *edtPinName0;
    QComboBox *cmbPB0Func;
    QComboBox *cmbPB2Func;
    QLabel *label_15;
    QLabel *label_14;
    QComboBox *cmbPB0Init;
    QLineEdit *edtPinName3;
    QLabel *label_6;
    QLabel *label_8;
    QComboBox *cmbPB1Func;
    QLabel *label_10;
    QLineEdit *edtPinName5;
    QComboBox *cmbPB3Init;
    QLabel *label_12;
    QComboBox *cmbPB3Func;
    QComboBox *cmbPB0Pushed;
    QComboBox *cmbPB1Pushed;
    QLineEdit *edtPinName4;
    QComboBox *cmbPB3Pushed;
    QLineEdit *edtPinName2;
    QComboBox *cmbPB2Pushed;
    QLabel *label_13;
    QLineEdit *edtPinName1;
    QLabel *label_17;
    QLabel *label_18;
    QHBoxLayout *horizontalLayout;
    QLabel *label_19;
    QLineEdit *edtReadInterval;
    QLabel *label_21;
    QSpacerItem *horizontalSpacer_2;
    QDialogButtonBox *btnOKCancel;

    void setupUi(QDialog *ConfigDialog)
    {
        if (ConfigDialog->objectName().isEmpty())
            ConfigDialog->setObjectName(QStringLiteral("ConfigDialog"));
        ConfigDialog->resize(512, 439);
        gridLayout_5 = new QGridLayout(ConfigDialog);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        tabWidget = new QTabWidget(ConfigDialog);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        gridLayout_13 = new QGridLayout(tab_3);
        gridLayout_13->setObjectName(QStringLiteral("gridLayout_13"));
        groupBox_5 = new QGroupBox(tab_3);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        gridLayout_17 = new QGridLayout(groupBox_5);
        gridLayout_17->setObjectName(QStringLiteral("gridLayout_17"));
        gridLayout_16 = new QGridLayout();
        gridLayout_16->setObjectName(QStringLiteral("gridLayout_16"));
        chkConnectOnBoot = new QCheckBox(groupBox_5);
        chkConnectOnBoot->setObjectName(QStringLiteral("chkConnectOnBoot"));

        gridLayout_16->addWidget(chkConnectOnBoot, 0, 0, 1, 1);

        chkExecTargetAfterWriting = new QCheckBox(groupBox_5);
        chkExecTargetAfterWriting->setObjectName(QStringLiteral("chkExecTargetAfterWriting"));

        gridLayout_16->addWidget(chkExecTargetAfterWriting, 1, 0, 1, 1);


        gridLayout_17->addLayout(gridLayout_16, 0, 0, 1, 1);


        gridLayout_13->addWidget(groupBox_5, 0, 0, 1, 1);

        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));

        gridLayout_13->addLayout(gridLayout_4, 1, 0, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_13->addItem(verticalSpacer_3, 2, 0, 1, 1);

        tabWidget->addTab(tab_3, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout_8 = new QGridLayout(tab);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        gridLayout_7 = new QGridLayout();
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        groupBox_3 = new QGroupBox(tab);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        gridLayout_9 = new QGridLayout(groupBox_3);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        cmbBaudRate = new QComboBox(groupBox_3);
        cmbBaudRate->setObjectName(QStringLiteral("cmbBaudRate"));

        gridLayout->addWidget(cmbBaudRate, 3, 1, 1, 1);

        cmbSynchMode = new QComboBox(groupBox_3);
        cmbSynchMode->setObjectName(QStringLiteral("cmbSynchMode"));

        gridLayout->addWidget(cmbSynchMode, 2, 1, 1, 1);

        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        label_2 = new QLabel(groupBox_3);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label = new QLabel(groupBox_3);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 2, 0, 1, 1);

        cmbComPort = new QComboBox(groupBox_3);
        cmbComPort->setObjectName(QStringLiteral("cmbComPort"));

        gridLayout->addWidget(cmbComPort, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 2, 1, 1);

        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 4, 2, 1, 1);

        edtSPIDelay = new QLineEdit(groupBox_3);
        edtSPIDelay->setObjectName(QStringLiteral("edtSPIDelay"));

        gridLayout->addWidget(edtSPIDelay, 4, 1, 1, 1);


        gridLayout_9->addLayout(gridLayout, 0, 0, 1, 1);


        gridLayout_7->addWidget(groupBox_3, 0, 0, 1, 1);


        gridLayout_8->addLayout(gridLayout_7, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_8->addItem(verticalSpacer, 1, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        gridLayout_11 = new QGridLayout(tab_2);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        gridLayout_10 = new QGridLayout();
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        groupBox_4 = new QGroupBox(tab_2);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout_2 = new QGridLayout(groupBox_4);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_12 = new QGridLayout();
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        label_16 = new QLabel(groupBox_4);
        label_16->setObjectName(QStringLiteral("label_16"));

        gridLayout_12->addWidget(label_16, 0, 0, 1, 1);

        cmbNewLine = new QComboBox(groupBox_4);
        cmbNewLine->setObjectName(QStringLiteral("cmbNewLine"));

        gridLayout_12->addWidget(cmbNewLine, 0, 1, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_12->addItem(horizontalSpacer_3, 0, 3, 1, 1);


        gridLayout_2->addLayout(gridLayout_12, 0, 0, 1, 1);


        gridLayout_10->addWidget(groupBox_4, 0, 0, 1, 1);


        gridLayout_11->addLayout(gridLayout_10, 0, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);

        gridLayout_11->addItem(verticalSpacer_2, 2, 0, 1, 1);

        groupBox_6 = new QGroupBox(tab_2);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        gridLayout_18 = new QGridLayout(groupBox_6);
        gridLayout_18->setObjectName(QStringLiteral("gridLayout_18"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label_9 = new QLabel(groupBox_6);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout_3->addWidget(label_9, 3, 0, 1, 1);

        cmbPB1Init = new QComboBox(groupBox_6);
        cmbPB1Init->setObjectName(QStringLiteral("cmbPB1Init"));

        gridLayout_3->addWidget(cmbPB1Init, 2, 3, 1, 1);

        cmbPB2Init = new QComboBox(groupBox_6);
        cmbPB2Init->setObjectName(QStringLiteral("cmbPB2Init"));

        gridLayout_3->addWidget(cmbPB2Init, 3, 3, 1, 1);

        edtPinName0 = new QLineEdit(groupBox_6);
        edtPinName0->setObjectName(QStringLiteral("edtPinName0"));

        gridLayout_3->addWidget(edtPinName0, 1, 1, 1, 1);

        cmbPB0Func = new QComboBox(groupBox_6);
        cmbPB0Func->setObjectName(QStringLiteral("cmbPB0Func"));

        gridLayout_3->addWidget(cmbPB0Func, 1, 2, 1, 1);

        cmbPB2Func = new QComboBox(groupBox_6);
        cmbPB2Func->setObjectName(QStringLiteral("cmbPB2Func"));

        gridLayout_3->addWidget(cmbPB2Func, 3, 2, 1, 1);

        label_15 = new QLabel(groupBox_6);
        label_15->setObjectName(QStringLiteral("label_15"));

        gridLayout_3->addWidget(label_15, 0, 1, 1, 1);

        label_14 = new QLabel(groupBox_6);
        label_14->setObjectName(QStringLiteral("label_14"));

        gridLayout_3->addWidget(label_14, 0, 2, 1, 1);

        cmbPB0Init = new QComboBox(groupBox_6);
        cmbPB0Init->setObjectName(QStringLiteral("cmbPB0Init"));

        gridLayout_3->addWidget(cmbPB0Init, 1, 3, 1, 1);

        edtPinName3 = new QLineEdit(groupBox_6);
        edtPinName3->setObjectName(QStringLiteral("edtPinName3"));

        gridLayout_3->addWidget(edtPinName3, 4, 1, 1, 1);

        label_6 = new QLabel(groupBox_6);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_3->addWidget(label_6, 1, 0, 1, 1);

        label_8 = new QLabel(groupBox_6);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout_3->addWidget(label_8, 2, 0, 1, 1);

        cmbPB1Func = new QComboBox(groupBox_6);
        cmbPB1Func->setObjectName(QStringLiteral("cmbPB1Func"));

        gridLayout_3->addWidget(cmbPB1Func, 2, 2, 1, 1);

        label_10 = new QLabel(groupBox_6);
        label_10->setObjectName(QStringLiteral("label_10"));

        gridLayout_3->addWidget(label_10, 4, 0, 1, 1);

        edtPinName5 = new QLineEdit(groupBox_6);
        edtPinName5->setObjectName(QStringLiteral("edtPinName5"));

        gridLayout_3->addWidget(edtPinName5, 6, 1, 1, 1);

        cmbPB3Init = new QComboBox(groupBox_6);
        cmbPB3Init->setObjectName(QStringLiteral("cmbPB3Init"));

        gridLayout_3->addWidget(cmbPB3Init, 4, 3, 1, 1);

        label_12 = new QLabel(groupBox_6);
        label_12->setObjectName(QStringLiteral("label_12"));

        gridLayout_3->addWidget(label_12, 0, 3, 1, 1);

        cmbPB3Func = new QComboBox(groupBox_6);
        cmbPB3Func->setObjectName(QStringLiteral("cmbPB3Func"));

        gridLayout_3->addWidget(cmbPB3Func, 4, 2, 1, 1);

        cmbPB0Pushed = new QComboBox(groupBox_6);
        cmbPB0Pushed->setObjectName(QStringLiteral("cmbPB0Pushed"));

        gridLayout_3->addWidget(cmbPB0Pushed, 1, 4, 1, 1);

        cmbPB1Pushed = new QComboBox(groupBox_6);
        cmbPB1Pushed->setObjectName(QStringLiteral("cmbPB1Pushed"));

        gridLayout_3->addWidget(cmbPB1Pushed, 2, 4, 1, 1);

        edtPinName4 = new QLineEdit(groupBox_6);
        edtPinName4->setObjectName(QStringLiteral("edtPinName4"));

        gridLayout_3->addWidget(edtPinName4, 5, 1, 1, 1);

        cmbPB3Pushed = new QComboBox(groupBox_6);
        cmbPB3Pushed->setObjectName(QStringLiteral("cmbPB3Pushed"));

        gridLayout_3->addWidget(cmbPB3Pushed, 4, 4, 1, 1);

        edtPinName2 = new QLineEdit(groupBox_6);
        edtPinName2->setObjectName(QStringLiteral("edtPinName2"));

        gridLayout_3->addWidget(edtPinName2, 3, 1, 1, 1);

        cmbPB2Pushed = new QComboBox(groupBox_6);
        cmbPB2Pushed->setObjectName(QStringLiteral("cmbPB2Pushed"));

        gridLayout_3->addWidget(cmbPB2Pushed, 3, 4, 1, 1);

        label_13 = new QLabel(groupBox_6);
        label_13->setObjectName(QStringLiteral("label_13"));

        gridLayout_3->addWidget(label_13, 0, 4, 1, 1);

        edtPinName1 = new QLineEdit(groupBox_6);
        edtPinName1->setObjectName(QStringLiteral("edtPinName1"));

        gridLayout_3->addWidget(edtPinName1, 2, 1, 1, 1);

        label_17 = new QLabel(groupBox_6);
        label_17->setObjectName(QStringLiteral("label_17"));

        gridLayout_3->addWidget(label_17, 5, 0, 1, 1);

        label_18 = new QLabel(groupBox_6);
        label_18->setObjectName(QStringLiteral("label_18"));

        gridLayout_3->addWidget(label_18, 6, 0, 1, 1);


        gridLayout_18->addLayout(gridLayout_3, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_19 = new QLabel(groupBox_6);
        label_19->setObjectName(QStringLiteral("label_19"));

        horizontalLayout->addWidget(label_19);

        edtReadInterval = new QLineEdit(groupBox_6);
        edtReadInterval->setObjectName(QStringLiteral("edtReadInterval"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(edtReadInterval->sizePolicy().hasHeightForWidth());
        edtReadInterval->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(edtReadInterval);

        label_21 = new QLabel(groupBox_6);
        label_21->setObjectName(QStringLiteral("label_21"));

        horizontalLayout->addWidget(label_21);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout_18->addLayout(horizontalLayout, 1, 0, 1, 1);


        gridLayout_11->addWidget(groupBox_6, 1, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());

        gridLayout_5->addWidget(tabWidget, 0, 0, 1, 1);

        btnOKCancel = new QDialogButtonBox(ConfigDialog);
        btnOKCancel->setObjectName(QStringLiteral("btnOKCancel"));
        btnOKCancel->setEnabled(true);
        btnOKCancel->setOrientation(Qt::Horizontal);
        btnOKCancel->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_5->addWidget(btnOKCancel, 1, 0, 1, 1);


        retranslateUi(ConfigDialog);
        QObject::connect(btnOKCancel, SIGNAL(accepted()), ConfigDialog, SLOT(accept()));
        QObject::connect(btnOKCancel, SIGNAL(rejected()), ConfigDialog, SLOT(reject()));

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(ConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *ConfigDialog)
    {
        ConfigDialog->setWindowTitle(QApplication::translate("ConfigDialog", "Dialog", 0));
        groupBox_5->setTitle(QApplication::translate("ConfigDialog", "\345\205\250\350\210\254", 0));
        chkConnectOnBoot->setText(QApplication::translate("ConfigDialog", "\350\265\267\345\213\225\343\201\250\345\220\214\346\231\202\343\201\253\343\203\226\343\203\252\343\203\203\343\202\270\343\201\253\346\216\245\347\266\232\343\201\231\343\202\213", 0));
        chkExecTargetAfterWriting->setText(QApplication::translate("ConfigDialog", "\346\233\270\343\201\215\350\276\274\343\201\277\345\256\214\344\272\206\345\276\214\343\202\277\343\203\274\343\202\262\343\203\203\343\203\210\343\202\222\350\265\267\345\213\225\343\201\231\343\202\213", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("ConfigDialog", "\345\205\250\350\210\254", 0));
        groupBox_3->setTitle(QApplication::translate("ConfigDialog", "\343\202\267\343\203\252\343\202\242\343\203\253\343\203\235\343\203\274\343\203\210", 0));
        label_3->setText(QApplication::translate("ConfigDialog", "\351\200\232\344\277\241\351\200\237\345\272\246", 0));
        label_2->setText(QApplication::translate("ConfigDialog", "COM\343\203\235\343\203\274\343\203\210\343\200\200\343\200\200\343\200\200\343\200\200\343\200\200", 0));
        label->setText(QApplication::translate("ConfigDialog", "\351\200\232\344\277\241\346\226\271\345\274\217", 0));
        label_5->setText(QApplication::translate("ConfigDialog", "SPI\343\203\242\343\203\274\343\203\211\343\203\207\343\202\243\343\203\254\343\202\244", 0));
        label_7->setText(QApplication::translate("ConfigDialog", "\343\203\236\343\202\244\343\202\257\343\203\255\347\247\222/bit", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("ConfigDialog", "\343\203\226\343\203\252\343\203\203\343\202\270", 0));
        groupBox_4->setTitle(QApplication::translate("ConfigDialog", "\343\203\226\343\203\252\343\203\203\343\202\270-\343\202\277\343\203\274\343\202\262\343\203\203\343\203\210\351\226\223\351\200\232\344\277\241", 0));
        label_16->setText(QApplication::translate("ConfigDialog", "\346\224\271\350\241\214\343\202\263\343\203\274\343\203\211", 0));
        groupBox_6->setTitle(QApplication::translate("ConfigDialog", "\343\203\224\343\203\263", 0));
        label_9->setText(QApplication::translate("ConfigDialog", "PORTB,2", 0));
        label_15->setText(QApplication::translate("ConfigDialog", "\345\220\215\347\247\260", 0));
        label_14->setText(QApplication::translate("ConfigDialog", "\346\251\237\350\203\275", 0));
        label_6->setText(QApplication::translate("ConfigDialog", "PORTB,0", 0));
        label_8->setText(QApplication::translate("ConfigDialog", "PORTB,1", 0));
        label_10->setText(QApplication::translate("ConfigDialog", "PORTB,3", 0));
        label_12->setText(QApplication::translate("ConfigDialog", "\345\210\235\346\234\237\345\200\244", 0));
        label_13->setText(QApplication::translate("ConfigDialog", "\346\212\274\344\270\213\346\231\202", 0));
        label_17->setText(QApplication::translate("ConfigDialog", "DSR", 0));
        label_18->setText(QApplication::translate("ConfigDialog", "CTS", 0));
        label_19->setText(QApplication::translate("ConfigDialog", "\350\252\255\343\201\277\345\217\226\343\202\212\343\202\244\343\203\263\343\202\277\343\203\274\343\203\220\343\203\253", 0));
        label_21->setText(QApplication::translate("ConfigDialog", "\343\203\237\343\203\252\347\247\222", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("ConfigDialog", "\343\202\277\343\203\274\343\202\262\343\203\203\343\203\210", 0));
    } // retranslateUi

};

namespace Ui {
    class ConfigDialog: public Ui_ConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGDIALOG_H
