#include "program.h"
#include <QtGui/QKeyEvent>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QTextCodec>
#include <QDebug>

QString path = QDir::currentPath()+"/processFile";
Program::Program(QWidget *parent) : QMainWindow(parent)
{
    setGeometry(0,0,QApplication::desktop()->width(),QApplication::desktop()->height());
    createActions();
    createMenus();
    codeEditor = new CodeEditor(this);
    setCentralWidget(codeEditor);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    if(files.isEmpty())
    {
        setWindowTitle(QString::fromLocal8Bit("未命名"));
    }
    else
    {
        setWindowTitle(files.at(0));
        loadFile(path+"/"+files.at(0));
    }

}

Program::~Program()
{
}

void Program::createActions()
{
    openAction = new QAction(QString::fromLocal8Bit("打开(F1)"),this);
    openAction->setShortcut(tr("F1"));
    openAction->setStatusTip("打开文件");
    connect(openAction, &QAction::triggered, this, &Program::showOpenFile);

    newAction = new QAction(QString::fromLocal8Bit("新建(F2)"),this);
    newAction->setShortcut(tr("F2"));
    newAction->setStatusTip("新建文件");
    connect(newAction, &QAction::triggered, this, &Program::showNewFile);

    delAction = new QAction(QString::fromLocal8Bit("删除(F3)"),this);
    delAction->setShortcut(tr("F3"));
    delAction->setStatusTip("删除文件");
    connect(delAction, &QAction::triggered, this, &Program::deleteFile);

    saveAction = new QAction(QString::fromLocal8Bit("保存(F4)"),this);
    saveAction->setShortcut(tr("F4"));
    saveAction->setStatusTip("保存文件");
    connect(saveAction, &QAction::triggered, this, &Program::saveFile);

}

void Program::createMenus()
{
    QMenuBar *myMenu = menuBar();
    myMenu->setStyleSheet("QMenuBar::item{\
      background-color:rgb(89,87,87);border:3px solid rgb(60,60,60);margin:2px 2px;}\
       QMenuBar::item:selected{background-color:rgb(235,110,36);}\
        QMenuBar::item:pressed{background-color:rgb(235,110,6);border:1px solid rgb(60,60,60);}");
    myMenu->addAction(openAction);
    myMenu->addAction(newAction);
    myMenu->addAction(delAction);
    myMenu->addAction(saveAction);
}

void Program::showNewFile()
{
    Program *newProgram = new Program;
    newProgram->show();
}

void Program::showOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,"open File",path);
    if (!fileName.isEmpty())
    {
        if (codeEditor->document()->isEmpty())
        {
            loadFile(fileName);
        }
        else
        {
            Program *newProgram = new Program;
            newProgram->show();
            newProgram->loadFile(fileName);
        }
    }
}

void Program::loadFile(QString filename)
{
    QFile file(filename);
    QTextStream textStream;
    if (file.open(QIODevice::ReadOnly))
    {
        textStream.setDevice(&file);
        while(!textStream.atEnd())
        {
            codeEditor->setPlainText(textStream.readAll());
        }
    }
    file.close();
}

void Program::deleteFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,"delete File",path);
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        file.remove();
    }
}

void Program::saveFile()
{

    QString filename = QFileDialog::getSaveFileName(this,"save File",path);
    if (filename == "")
    {
        return;
    }
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "error", "Open File Error!");
        return;
    }
    else
    {
        QTextStream textStream(&file);
        QString str = codeEditor->toPlainText();
        textStream << str;
        lastFileContent = str;
    }
    QMessageBox::information(this, "Ssve File", "Save File Success", QMessageBox::Ok);
    file.close();
    lastFileName = filename;
}

void Program::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    if (e->key() == Qt::Key_F8)
    {
    }
    else if ((e->modifiers() == Qt::AltModifier) && (e->key() == Qt::Key_X))
    {
    }
}
