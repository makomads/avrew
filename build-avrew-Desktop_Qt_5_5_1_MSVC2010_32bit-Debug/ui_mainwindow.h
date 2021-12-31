/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionConfig;
    QAction *actionExit;
    QAction *actionAbout;
    QWidget *centralWidget;
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *btnTestConnection;
    QLabel *label_9;
    QComboBox *cmbDevice;
    QLabel *txtDevice;
    QFrame *line;
    QSplitter *splMainV;
    QSplitter *splMainH;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnWrite;
    QPushButton *btnSelectFlashWrite;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btnFlashRead;
    QLineEdit *edtFlashRead;
    QPushButton *btnSelectFlashRead;
    QHBoxLayout *horizontalLayout_5;
    QLineEdit *edtFlashWrite;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *edtEEPWrite;
    QPushButton *btnSelectEEPWrite;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_5;
    QPushButton *btnEEPRead;
    QLabel *label_6;
    QLineEdit *edtEEPRead;
    QPushButton *btnSelectEEPRead;
    QCheckBox *chkEEPWrite;
    QLabel *label_3;
    QCheckBox *chkVerify;
    QFrame *line_2;
    QSpacerItem *horizontalSpacer;
    QLabel *label_8;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *btnExecTarget;
    QPushButton *btnResetTarget;
    QSpacerItem *verticalSpacer;
    QGridLayout *grdPins;
    QLabel *label_4;
    QLabel *label_10;
    QHBoxLayout *loSendMessage;
    QLabel *label_11;
    QLineEdit *edtSendMessage;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QPushButton *btnWriteFuse;
    QPushButton *btnReadFuse;
    QScrollArea *scrFuses;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *scrFusesLayout;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout_2;
    QHBoxLayout *loConsole;
    QTextEdit *edtConsole;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_12;
    QCheckBox *chkShowReceived;
    QCheckBox *chkShowSended;
    QCheckBox *chkShowAppMessage;
    QCheckBox *chkShowError;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btnClearConsole;
    QMenuBar *menuBar;
    QMenu *menu_H;
    QMenu *menu_S;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(737, 569);
        actionConfig = new QAction(MainWindow);
        actionConfig->setObjectName(QStringLiteral("actionConfig"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        centralWidget->setAutoFillBackground(false);
        gridLayout_3 = new QGridLayout(centralWidget);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        btnTestConnection = new QPushButton(centralWidget);
        btnTestConnection->setObjectName(QStringLiteral("btnTestConnection"));

        horizontalLayout_4->addWidget(btnTestConnection);

        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QStringLiteral("label_9"));

        horizontalLayout_4->addWidget(label_9);

        cmbDevice = new QComboBox(centralWidget);
        cmbDevice->setObjectName(QStringLiteral("cmbDevice"));

        horizontalLayout_4->addWidget(cmbDevice);

        txtDevice = new QLabel(centralWidget);
        txtDevice->setObjectName(QStringLiteral("txtDevice"));

        horizontalLayout_4->addWidget(txtDevice);

        horizontalLayout_4->setStretch(3, 1);

        verticalLayout->addLayout(horizontalLayout_4);


        gridLayout_3->addLayout(verticalLayout, 0, 0, 1, 1);

        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_3->addWidget(line, 1, 0, 1, 1);

        splMainV = new QSplitter(centralWidget);
        splMainV->setObjectName(QStringLiteral("splMainV"));
        splMainV->setOrientation(Qt::Vertical);
        splMainH = new QSplitter(splMainV);
        splMainH->setObjectName(QStringLiteral("splMainH"));
        splMainH->setOrientation(Qt::Horizontal);
        verticalLayoutWidget_2 = new QWidget(splMainH);
        verticalLayoutWidget_2->setObjectName(QStringLiteral("verticalLayoutWidget_2"));
        verticalLayout_3 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(verticalLayoutWidget_2);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        btnWrite = new QPushButton(verticalLayoutWidget_2);
        btnWrite->setObjectName(QStringLiteral("btnWrite"));

        horizontalLayout->addWidget(btnWrite);


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 1);

        btnSelectFlashWrite = new QPushButton(verticalLayoutWidget_2);
        btnSelectFlashWrite->setObjectName(QStringLiteral("btnSelectFlashWrite"));
        btnSelectFlashWrite->setMaximumSize(QSize(35, 16777215));

        gridLayout->addWidget(btnSelectFlashWrite, 1, 3, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        btnFlashRead = new QPushButton(verticalLayoutWidget_2);
        btnFlashRead->setObjectName(QStringLiteral("btnFlashRead"));

        horizontalLayout_2->addWidget(btnFlashRead);


        gridLayout->addLayout(horizontalLayout_2, 5, 0, 1, 1);

        edtFlashRead = new QLineEdit(verticalLayoutWidget_2);
        edtFlashRead->setObjectName(QStringLiteral("edtFlashRead"));

        gridLayout->addWidget(edtFlashRead, 5, 2, 1, 1);

        btnSelectFlashRead = new QPushButton(verticalLayoutWidget_2);
        btnSelectFlashRead->setObjectName(QStringLiteral("btnSelectFlashRead"));
        btnSelectFlashRead->setMaximumSize(QSize(35, 16777215));

        gridLayout->addWidget(btnSelectFlashRead, 5, 3, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        edtFlashWrite = new QLineEdit(verticalLayoutWidget_2);
        edtFlashWrite->setObjectName(QStringLiteral("edtFlashWrite"));

        horizontalLayout_5->addWidget(edtFlashWrite);


        gridLayout->addLayout(horizontalLayout_5, 1, 2, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        edtEEPWrite = new QLineEdit(verticalLayoutWidget_2);
        edtEEPWrite->setObjectName(QStringLiteral("edtEEPWrite"));

        horizontalLayout_3->addWidget(edtEEPWrite);


        gridLayout->addLayout(horizontalLayout_3, 2, 2, 1, 1);

        btnSelectEEPWrite = new QPushButton(verticalLayoutWidget_2);
        btnSelectEEPWrite->setObjectName(QStringLiteral("btnSelectEEPWrite"));
        btnSelectEEPWrite->setMaximumSize(QSize(35, 16777215));

        gridLayout->addWidget(btnSelectEEPWrite, 2, 3, 1, 1);

        label_2 = new QLabel(verticalLayoutWidget_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 4, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 13, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 3, 1, 1, 1);

        label_5 = new QLabel(verticalLayoutWidget_2);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 5, 1, 1, 1);

        btnEEPRead = new QPushButton(verticalLayoutWidget_2);
        btnEEPRead->setObjectName(QStringLiteral("btnEEPRead"));

        gridLayout->addWidget(btnEEPRead, 6, 0, 1, 1);

        label_6 = new QLabel(verticalLayoutWidget_2);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 6, 1, 1, 1);

        edtEEPRead = new QLineEdit(verticalLayoutWidget_2);
        edtEEPRead->setObjectName(QStringLiteral("edtEEPRead"));

        gridLayout->addWidget(edtEEPRead, 6, 2, 1, 1);

        btnSelectEEPRead = new QPushButton(verticalLayoutWidget_2);
        btnSelectEEPRead->setObjectName(QStringLiteral("btnSelectEEPRead"));
        btnSelectEEPRead->setMaximumSize(QSize(35, 16777215));

        gridLayout->addWidget(btnSelectEEPRead, 6, 3, 1, 1);

        chkEEPWrite = new QCheckBox(verticalLayoutWidget_2);
        chkEEPWrite->setObjectName(QStringLiteral("chkEEPWrite"));

        gridLayout->addWidget(chkEEPWrite, 2, 1, 1, 1);

        label_3 = new QLabel(verticalLayoutWidget_2);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 1, 1, 1, 1);

        chkVerify = new QCheckBox(verticalLayoutWidget_2);
        chkVerify->setObjectName(QStringLiteral("chkVerify"));

        gridLayout->addWidget(chkVerify, 2, 0, 1, 1);

        gridLayout->setColumnStretch(2, 1);

        verticalLayout_3->addLayout(gridLayout);

        line_2 = new QFrame(verticalLayoutWidget_2);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_3->addWidget(line_2);

        horizontalSpacer = new QSpacerItem(40, 13, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout_3->addItem(horizontalSpacer);

        label_8 = new QLabel(verticalLayoutWidget_2);
        label_8->setObjectName(QStringLiteral("label_8"));

        verticalLayout_3->addWidget(label_8);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        btnExecTarget = new QPushButton(verticalLayoutWidget_2);
        btnExecTarget->setObjectName(QStringLiteral("btnExecTarget"));
        btnExecTarget->setCheckable(true);

        horizontalLayout_7->addWidget(btnExecTarget);

        btnResetTarget = new QPushButton(verticalLayoutWidget_2);
        btnResetTarget->setObjectName(QStringLiteral("btnResetTarget"));

        horizontalLayout_7->addWidget(btnResetTarget);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        horizontalLayout_7->addItem(verticalSpacer);

        grdPins = new QGridLayout();
        grdPins->setSpacing(6);
        grdPins->setObjectName(QStringLiteral("grdPins"));
        label_4 = new QLabel(verticalLayoutWidget_2);
        label_4->setObjectName(QStringLiteral("label_4"));

        grdPins->addWidget(label_4, 0, 0, 1, 1);

        label_10 = new QLabel(verticalLayoutWidget_2);
        label_10->setObjectName(QStringLiteral("label_10"));

        grdPins->addWidget(label_10, 1, 0, 1, 1);


        horizontalLayout_7->addLayout(grdPins);

        horizontalLayout_7->setStretch(2, 1);

        verticalLayout_3->addLayout(horizontalLayout_7);

        loSendMessage = new QHBoxLayout();
        loSendMessage->setSpacing(6);
        loSendMessage->setObjectName(QStringLiteral("loSendMessage"));
        label_11 = new QLabel(verticalLayoutWidget_2);
        label_11->setObjectName(QStringLiteral("label_11"));

        loSendMessage->addWidget(label_11);

        edtSendMessage = new QLineEdit(verticalLayoutWidget_2);
        edtSendMessage->setObjectName(QStringLiteral("edtSendMessage"));

        loSendMessage->addWidget(edtSendMessage);


        verticalLayout_3->addLayout(loSendMessage);

        verticalLayout_3->setStretch(2, 1);
        splMainH->addWidget(verticalLayoutWidget_2);
        verticalLayoutWidget = new QWidget(splMainH);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_7 = new QLabel(verticalLayoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_6->addWidget(label_7);

        btnWriteFuse = new QPushButton(verticalLayoutWidget);
        btnWriteFuse->setObjectName(QStringLiteral("btnWriteFuse"));

        horizontalLayout_6->addWidget(btnWriteFuse);

        btnReadFuse = new QPushButton(verticalLayoutWidget);
        btnReadFuse->setObjectName(QStringLiteral("btnReadFuse"));

        horizontalLayout_6->addWidget(btnReadFuse);

        horizontalLayout_6->setStretch(0, 1);

        verticalLayout_2->addLayout(horizontalLayout_6);

        scrFuses = new QScrollArea(verticalLayoutWidget);
        scrFuses->setObjectName(QStringLiteral("scrFuses"));
        scrFuses->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 253, 252));
        verticalLayout_4 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        scrFusesLayout = new QVBoxLayout();
        scrFusesLayout->setSpacing(6);
        scrFusesLayout->setObjectName(QStringLiteral("scrFusesLayout"));

        verticalLayout_4->addLayout(scrFusesLayout);

        scrFuses->setWidget(scrollAreaWidgetContents);

        verticalLayout_2->addWidget(scrFuses);

        splMainH->addWidget(verticalLayoutWidget);
        splMainV->addWidget(splMainH);
        gridLayoutWidget = new QWidget(splMainV);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayout_2 = new QGridLayout(gridLayoutWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        loConsole = new QHBoxLayout();
        loConsole->setSpacing(6);
        loConsole->setObjectName(QStringLiteral("loConsole"));
        edtConsole = new QTextEdit(gridLayoutWidget);
        edtConsole->setObjectName(QStringLiteral("edtConsole"));

        loConsole->addWidget(edtConsole);


        gridLayout_2->addLayout(loConsole, 2, 0, 1, 1);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        label_12 = new QLabel(gridLayoutWidget);
        label_12->setObjectName(QStringLiteral("label_12"));

        horizontalLayout_9->addWidget(label_12);

        chkShowReceived = new QCheckBox(gridLayoutWidget);
        chkShowReceived->setObjectName(QStringLiteral("chkShowReceived"));

        horizontalLayout_9->addWidget(chkShowReceived);

        chkShowSended = new QCheckBox(gridLayoutWidget);
        chkShowSended->setObjectName(QStringLiteral("chkShowSended"));

        horizontalLayout_9->addWidget(chkShowSended);

        chkShowAppMessage = new QCheckBox(gridLayoutWidget);
        chkShowAppMessage->setObjectName(QStringLiteral("chkShowAppMessage"));

        horizontalLayout_9->addWidget(chkShowAppMessage);

        chkShowError = new QCheckBox(gridLayoutWidget);
        chkShowError->setObjectName(QStringLiteral("chkShowError"));

        horizontalLayout_9->addWidget(chkShowError);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_3);

        btnClearConsole = new QPushButton(gridLayoutWidget);
        btnClearConsole->setObjectName(QStringLiteral("btnClearConsole"));

        horizontalLayout_9->addWidget(btnClearConsole);


        gridLayout_2->addLayout(horizontalLayout_9, 1, 0, 1, 1);

        splMainV->addWidget(gridLayoutWidget);

        gridLayout_3->addWidget(splMainV, 2, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 737, 21));
        menu_H = new QMenu(menuBar);
        menu_H->setObjectName(QStringLiteral("menu_H"));
        menu_S = new QMenu(menuBar);
        menu_S->setObjectName(QStringLiteral("menu_S"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu_S->menuAction());
        menuBar->addAction(menu_H->menuAction());
        menu_H->addAction(actionAbout);
        menu_S->addAction(actionConfig);
        menu_S->addAction(actionExit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionConfig->setText(QApplication::translate("MainWindow", "\350\250\255\345\256\232", 0));
        actionExit->setText(QApplication::translate("MainWindow", "\347\265\202\344\272\206", 0));
        actionAbout->setText(QApplication::translate("MainWindow", "\343\201\223\343\201\256\343\202\275\343\203\225\343\203\210\343\201\253\343\201\244\343\201\204\343\201\246", 0));
        btnTestConnection->setText(QApplication::translate("MainWindow", "\346\216\245\347\266\232\343\203\206\343\202\271\343\203\210", 0));
        label_9->setText(QApplication::translate("MainWindow", "\343\203\207\343\203\220\343\202\244\343\202\271", 0));
        txtDevice->setText(QApplication::translate("MainWindow", "TextLabel", 0));
        label->setText(QApplication::translate("MainWindow", "\346\233\270\343\201\215\350\276\274\343\201\277", 0));
        btnWrite->setText(QApplication::translate("MainWindow", "\346\233\270\343\201\215\350\276\274\343\201\277", 0));
        btnSelectFlashWrite->setText(QApplication::translate("MainWindow", "...", 0));
        btnFlashRead->setText(QApplication::translate("MainWindow", "FLASH\350\252\255\343\201\277\345\207\272\343\201\227", 0));
        btnSelectFlashRead->setText(QApplication::translate("MainWindow", "...", 0));
        btnSelectEEPWrite->setText(QApplication::translate("MainWindow", "...", 0));
        label_2->setText(QApplication::translate("MainWindow", "\350\252\255\343\201\277\345\207\272\343\201\227", 0));
        label_5->setText(QApplication::translate("MainWindow", "FLASH", 0));
        btnEEPRead->setText(QApplication::translate("MainWindow", "EEPROM\350\252\255\343\201\277\345\207\272\343\201\227", 0));
        label_6->setText(QApplication::translate("MainWindow", "EEPROM", 0));
        btnSelectEEPRead->setText(QApplication::translate("MainWindow", "...", 0));
        chkEEPWrite->setText(QApplication::translate("MainWindow", "EEPROM", 0));
        label_3->setText(QApplication::translate("MainWindow", "FLASH", 0));
        chkVerify->setText(QApplication::translate("MainWindow", "\347\205\247\345\220\210", 0));
        label_8->setText(QApplication::translate("MainWindow", "\343\202\277\343\203\274\343\202\262\343\203\203\343\203\210\351\200\232\344\277\241", 0));
        btnExecTarget->setText(QApplication::translate("MainWindow", "\343\202\277\343\203\274\343\202\262\343\203\203\343\203\210\350\265\267\345\213\225", 0));
        btnResetTarget->setText(QApplication::translate("MainWindow", "\343\203\252\343\202\273\343\203\203\343\203\210", 0));
        label_4->setText(QString());
        label_10->setText(QString());
        label_11->setText(QApplication::translate("MainWindow", "\351\200\201\344\277\241", 0));
        label_7->setText(QApplication::translate("MainWindow", "\343\203\222\343\203\245\343\203\274\343\202\272", 0));
        btnWriteFuse->setText(QApplication::translate("MainWindow", "\346\233\270\343\201\215\350\276\274\343\201\277", 0));
        btnReadFuse->setText(QApplication::translate("MainWindow", "\350\252\255\343\201\277\345\207\272\343\201\227", 0));
        label_12->setText(QApplication::translate("MainWindow", "\350\241\250\347\244\272\351\240\205\347\233\256", 0));
        chkShowReceived->setText(QApplication::translate("MainWindow", "\345\217\227\344\277\241\343\203\241\343\203\203\343\202\273\343\203\274\343\202\270", 0));
        chkShowSended->setText(QApplication::translate("MainWindow", "\351\200\201\344\277\241\343\202\250\343\202\263\343\203\274", 0));
        chkShowAppMessage->setText(QApplication::translate("MainWindow", "\343\202\242\343\203\227\343\203\252\343\202\261\343\203\274\343\202\267\343\203\247\343\203\263\343\203\241\343\203\203\343\202\273\343\203\274\343\202\270", 0));
        chkShowError->setText(QApplication::translate("MainWindow", "\343\202\250\343\203\251\343\203\274", 0));
        btnClearConsole->setText(QApplication::translate("MainWindow", "\343\202\257\343\203\252\343\202\242", 0));
        menu_H->setTitle(QApplication::translate("MainWindow", "\343\203\230\343\203\253\343\203\227(&H)", 0));
        menu_S->setTitle(QApplication::translate("MainWindow", "\343\203\225\343\202\241\343\202\244\343\203\253(&F)", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
