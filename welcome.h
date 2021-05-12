#ifndef WELCOME_H
#define WELCOME_H

#include <QWidget>

class GLWidget;

class Welcome : public QWidget
{
    Q_OBJECT

public:
    Welcome();

private slots:
    void rotateOneStep();

private:
    GLWidget *glWidgets;
};

#endif
