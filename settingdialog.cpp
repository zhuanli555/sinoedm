#include "settingdialog.h"
#include "ui_settingdialog.h"
#include <QTextCodec>
#include <QDebug>
#include <QFile>

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    QFile file("./db/edmsystem.ini");
    if(!file.exists())
    {
        setting = new QSettings("./db/edmsystem.ini",QSettings::IniFormat);
        setting->setIniCodec(QTextCodec::codecForName("utf-8"));
        setting ->beginGroup("systemSet");
        setting->setValue("xLabelFlag",1);
        setting->setValue("yLabelFlag",1);
        setting->setValue("aLabelFlag",1);
        setting->setValue("bLabelFlag",1);
        setting->setValue("xMValueLimit",0);
        setting->setValue("yMValueLimit",0);
        setting->setValue("wMValueLimit",0);
        setting->endGroup();
    }
    setting = new QSettings("./db/edmsystem.ini",QSettings::IniFormat);
    setting->setIniCodec(QTextCodec::codecForName("utf-8"));
    setting ->beginGroup("systemSet");
    int xLabelFlag,yLabelFlag,aLabelFlag,bLabelFlag=0;
    xLabelFlag = setting->value("xLabelFlag").toInt();
    yLabelFlag = setting->value("yLabelFlag").toInt();
    aLabelFlag = setting->value("aLabelFlag").toInt();
    bLabelFlag = setting->value("bLabelFlag").toInt();
    ui->xLabelFlag->setChecked(xLabelFlag);
    ui->yLabelFlag->setChecked(yLabelFlag);
    ui->aLabelFlag->setChecked(aLabelFlag);
    ui->bLabelFlag->setChecked(bLabelFlag);
    ui->xMValueLimit->setText(setting->value("xMValueLimit").toString());
    ui->yMValueLimit->setText(setting->value("yMValueLimit").toString());
    ui->wMValueLimit->setText(setting->value("wMValueLimit").toString());
    setting->endGroup();
}


SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::on_buttonBox_accepted()
{
    setting = new QSettings("./db/edmsystem.ini",QSettings::IniFormat);
    setting->setIniCodec(QTextCodec::codecForName("utf-8"));
    int xLabelFlag,yLabelFlag,aLabelFlag,bLabelFlag=0;
    QString xMValueLimit,yMValueLimit,wMValueLimit;
    xLabelFlag = ui->xLabelFlag->isChecked()?1:0;
    yLabelFlag = ui->yLabelFlag->isChecked()?1:0;
    aLabelFlag = ui->aLabelFlag->isChecked()?1:0;
    bLabelFlag = ui->bLabelFlag->isChecked()?1:0;
    xMValueLimit = ui->xMValueLimit->text();
    yMValueLimit = ui->yMValueLimit->text();
    wMValueLimit = ui->wMValueLimit->text();
    setting->beginGroup("systemSet");
    setting->setValue("xLabelFlag",xLabelFlag);
    setting->setValue("yLabelFlag",yLabelFlag);
    setting->setValue("aLabelFlag",aLabelFlag);
    setting->setValue("bLabelFlag",bLabelFlag);
    setting->setValue("xMValueLimit",xMValueLimit);
    setting->setValue("yMValueLimit",yMValueLimit);
    setting->setValue("wMValueLimit",wMValueLimit);
    setting->endGroup();
    emit systemSetChanged();
    return QDialog::accept();
}
