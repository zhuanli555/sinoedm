#ifndef PROGRAM_H
#define PROGRAM_H

#include <QMainWindow>
#include <QWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QLabel>
#include "codeeditor.h"

class Program : public QMainWindow
{
    Q_OBJECT
public:
    explicit Program(QWidget *parent = nullptr);
    ~Program();
    void createMenus();
    void createActions();
    void loadFile(QString filename);
public:
    int openFileFlag = 0;
    int newFileFlag = 0;
    QString lastFileName;
    QString lastFileContent;

private:
    QAction* openAction;
    QAction* newAction;
    QAction* delAction;
    QAction* saveAction;
    CodeEditor* codeEditor;

protected:
    void keyPressEvent(QKeyEvent *) override;

public slots:
    void showOpenFile();
    void showNewFile();
    void deleteFile();
    void saveFile();
};

#endif // PROGRAM_H
