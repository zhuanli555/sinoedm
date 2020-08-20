#include "setting.h"
#include <QtGui/QKeyEvent>

Setting::Setting(QWidget *parent) : QWidget(parent)
{
    setGeometry(0,0,800,600);
}

Setting::~Setting()
{

}

void Setting::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    if (e->key() == Qt::Key_F8){
    }
    else if((e->modifiers() == Qt::AltModifier) && (e->key() == Qt::Key_X)){
    }
}
