#include <QtWidgets>

#include "glwidget.h"
#include "welcome.h"

Welcome::Welcome()
{
    showFullScreen();
    QGridLayout *mainLayout = new QGridLayout;
    QColor clearColor;
    clearColor.setHsv(0,255, 63);
    glWidgets = new GLWidget;
    glWidgets->setClearColor(clearColor);
    glWidgets->rotateBy(+42 * 16, +42 * 16, -21 * 16);
    mainLayout->addWidget(glWidgets, 0, 0);
    connect(glWidgets, &GLWidget::clicked,
            this, &Welcome::close);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Welcome::rotateOneStep);
    timer->start(20);
    setLayout(mainLayout);
    setWindowTitle(tr("welcome!welcome!welcome!"));
}


void Welcome::rotateOneStep()
{
    static int i=0;
    if(i++>250)close();
    if (glWidgets)
        glWidgets->rotateBy(+2 * 16, +2 * 16, -1 * 16);
}
