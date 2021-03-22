/********************************************************************************
** Form generated from reading UI file 'settingdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGDIALOG_H
#define UI_SETTINGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingDialog
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *label;
    QCheckBox *xLabelFlag;
    QLabel *label_2;
    QCheckBox *yLabelFlag;
    QLabel *label_3;
    QCheckBox *aLabelFlag;
    QLabel *label_4;
    QCheckBox *bLabelFlag;
    QLabel *label_5;
    QLineEdit *xMValueLimit;
    QLabel *label_6;
    QLineEdit *yMValueLimit;
    QLabel *label_7;
    QLineEdit *wMValueLimit;

    void setupUi(QDialog *SettingDialog)
    {
        if (SettingDialog->objectName().isEmpty())
            SettingDialog->setObjectName(QString::fromUtf8("SettingDialog"));
        SettingDialog->resize(480, 640);
        buttonBox = new QDialogButtonBox(SettingDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 600, 461, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        widget = new QWidget(SettingDialog);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(90, 60, 201, 223));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        xLabelFlag = new QCheckBox(widget);
        xLabelFlag->setObjectName(QString::fromUtf8("xLabelFlag"));
        xLabelFlag->setChecked(true);

        gridLayout->addWidget(xLabelFlag, 0, 1, 1, 1);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        yLabelFlag = new QCheckBox(widget);
        yLabelFlag->setObjectName(QString::fromUtf8("yLabelFlag"));
        yLabelFlag->setChecked(true);

        gridLayout->addWidget(yLabelFlag, 1, 1, 1, 1);

        label_3 = new QLabel(widget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        aLabelFlag = new QCheckBox(widget);
        aLabelFlag->setObjectName(QString::fromUtf8("aLabelFlag"));

        gridLayout->addWidget(aLabelFlag, 2, 1, 1, 1);

        label_4 = new QLabel(widget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        bLabelFlag = new QCheckBox(widget);
        bLabelFlag->setObjectName(QString::fromUtf8("bLabelFlag"));

        gridLayout->addWidget(bLabelFlag, 3, 1, 1, 1);

        label_5 = new QLabel(widget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        xMValueLimit = new QLineEdit(widget);
        xMValueLimit->setObjectName(QString::fromUtf8("xMValueLimit"));

        gridLayout->addWidget(xMValueLimit, 4, 1, 1, 1);

        label_6 = new QLabel(widget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 5, 0, 1, 1);

        yMValueLimit = new QLineEdit(widget);
        yMValueLimit->setObjectName(QString::fromUtf8("yMValueLimit"));

        gridLayout->addWidget(yMValueLimit, 5, 1, 1, 1);

        label_7 = new QLabel(widget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 6, 0, 1, 1);

        wMValueLimit = new QLineEdit(widget);
        wMValueLimit->setObjectName(QString::fromUtf8("wMValueLimit"));

        gridLayout->addWidget(wMValueLimit, 6, 1, 1, 1);

        yLabelFlag->raise();
        buttonBox->raise();
        label->raise();
        label_2->raise();
        label_3->raise();
        label_4->raise();
        label_5->raise();
        label_6->raise();
        xLabelFlag->raise();
        label_7->raise();
        bLabelFlag->raise();
        aLabelFlag->raise();
        xMValueLimit->raise();
        yMValueLimit->raise();
        wMValueLimit->raise();

        retranslateUi(SettingDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SettingDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SettingDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SettingDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingDialog)
    {
        SettingDialog->setWindowTitle(QApplication::translate("SettingDialog", "Dialog", nullptr));
        label->setText(QApplication::translate("SettingDialog", "X\350\275\264\351\205\215\347\275\256", nullptr));
        xLabelFlag->setText(QString());
        label_2->setText(QApplication::translate("SettingDialog", "Y\350\275\264\351\205\215\347\275\256", nullptr));
        yLabelFlag->setText(QString());
        label_3->setText(QApplication::translate("SettingDialog", "A\350\275\264\351\205\215\347\275\256", nullptr));
        aLabelFlag->setText(QString());
        label_4->setText(QApplication::translate("SettingDialog", "B\350\275\264\351\205\215\347\275\256", nullptr));
        bLabelFlag->setText(QString());
        label_5->setText(QApplication::translate("SettingDialog", "X\350\275\264\350\241\214\347\250\213", nullptr));
        xMValueLimit->setText(QApplication::translate("SettingDialog", "0", nullptr));
        label_6->setText(QApplication::translate("SettingDialog", "Y\350\275\264\350\241\214\347\250\213", nullptr));
        yMValueLimit->setText(QApplication::translate("SettingDialog", "0", nullptr));
        label_7->setText(QApplication::translate("SettingDialog", "W\350\275\264\350\241\214\347\250\213", nullptr));
        wMValueLimit->setText(QApplication::translate("SettingDialog", "0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingDialog: public Ui_SettingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGDIALOG_H
