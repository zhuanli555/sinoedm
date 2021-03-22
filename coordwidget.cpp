#include "coordwidget.h"
#include <QDebug>
#include <QPixmap>
#include <QSettings>

CoordWidget* CoordWidget::m_coordWid = nullptr;
CoordWidget::CoordWidget(QWidget *parent) : QWidget(parent)
{
    edm = EDM::GetEdmInstance();
    setMaximumWidth(600);
    setLabels();
}

CoordWidget::~CoordWidget()
{

}
void CoordWidget::setLabels()
{
    xLabel = new QLabel("X");
    yLabel = new QLabel("Y");
    cLabel = new QLabel("C");
    wLabel = new QLabel("W");
    aLabel = new QLabel("A");
    bLabel = new QLabel("B");
    zLabel = new QLabel("Z");
    float zero = 0.000;
    QString tmp1 = QString("%1").arg("-123.567");
    QString tmp2 = QString("%1").arg(zero,8,'f',3);
    xValue = new QLabel(tmp1);
    yValue = new QLabel(tmp2);
    cValue = new QLabel(tmp2);
    zValue = new QLabel(tmp2);
    xMValue = new QLabel(tmp1);
    yMValue = new QLabel(tmp2);
    cMValue = new QLabel(tmp2);
    zMValue = new QLabel(tmp2);

    wValue = new QLabel(tmp1);
    aValue = new QLabel(tmp2);
    bValue = new QLabel(tmp2);
    wMValue = new QLabel(tmp1);
    aMValue = new QLabel(tmp2);
    bMValue = new QLabel(tmp2);

    xShow = new QLabel();
    yShow = new QLabel();
    cShow = new QLabel();
    wShow = new QLabel();
    aShow = new QLabel();
    bShow = new QLabel();
    zShow = new QLabel();

    xLabel->setAlignment(Qt::AlignVCenter);
    yLabel->setAlignment(Qt::AlignVCenter);
    cLabel->setAlignment(Qt::AlignVCenter);
    wLabel->setAlignment(Qt::AlignVCenter);
    aLabel->setAlignment(Qt::AlignVCenter);
    bLabel->setAlignment(Qt::AlignVCenter);
    zLabel->setAlignment(Qt::AlignVCenter);
    xValue->setAlignment(Qt::AlignRight);
    yValue->setAlignment(Qt::AlignRight);
    cValue->setAlignment(Qt::AlignRight);
    wValue->setAlignment(Qt::AlignRight);
    aValue->setAlignment(Qt::AlignRight);
    bValue->setAlignment(Qt::AlignRight);
    zValue->setAlignment(Qt::AlignRight);
    xMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    yMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    cMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    wMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    aMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    bMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    zMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

    leftLayout = new QGridLayout(this);
    leftLayout->addWidget(xLabel,0,0);
    leftLayout->addWidget(xValue,0,1);
    leftLayout->addWidget(xMValue,0,2);leftLayout->addWidget(xShow,0,3);
    leftLayout->addWidget(yLabel,1,0);
    leftLayout->addWidget(yValue,1,1);
    leftLayout->addWidget(yMValue,1,2);leftLayout->addWidget(yShow,1,3);
    leftLayout->addWidget(cLabel,2,0);
    leftLayout->addWidget(cValue,2,1);
    leftLayout->addWidget(cMValue,2,2);leftLayout->addWidget(cShow,2,3);
    leftLayout->addWidget(wLabel,3,0);
    leftLayout->addWidget(wValue,3,1);
    leftLayout->addWidget(wMValue,3,2);leftLayout->addWidget(wShow,3,3);
    leftLayout->addWidget(aLabel,4,0);
    leftLayout->addWidget(aValue,4,1);
    leftLayout->addWidget(aMValue,4,2);leftLayout->addWidget(aShow,4,3);
    leftLayout->addWidget(bLabel,5,0);
    leftLayout->addWidget(bValue,5,1);
    leftLayout->addWidget(bMValue,5,2);leftLayout->addWidget(bShow,5,3);
    leftLayout->addWidget(zLabel,6,0);
    leftLayout->addWidget(zValue,6,1);
    leftLayout->addWidget(zMValue,6,2);leftLayout->addWidget(zShow,6,3);
    //setStyleSheet("QLabel{background-color:yellow}");
    QString xstyle = "font-size:40px;";
    QString wstyle = "font-size:28px;";
    //bShow->setStyleSheet("border-image:url(zx.png);")
    xLabel->setStyleSheet(xstyle);
    yLabel->setStyleSheet(xstyle);
    cLabel->setStyleSheet(xstyle);
    wLabel->setStyleSheet(wstyle);
    aLabel->setStyleSheet(xstyle);
    bLabel->setStyleSheet(xstyle);
    zLabel->setStyleSheet(xstyle);
    xValue->setStyleSheet(xstyle);
    yValue->setStyleSheet(xstyle);
    cValue->setStyleSheet(xstyle);
    wValue->setStyleSheet(xstyle);
    aValue->setStyleSheet(xstyle);
    bValue->setStyleSheet(xstyle);
    zValue->setStyleSheet(xstyle);
    xMValue->setStyleSheet("color:blue;font-size:24px;");
    yMValue->setStyleSheet("color:blue;font-size:24px;");
    cMValue->setStyleSheet("color:blue;font-size:24px;");
    wMValue->setStyleSheet("color:blue;font-size:24px;");
    aMValue->setStyleSheet("color:blue;font-size:24px;");
    bMValue->setStyleSheet("color:blue;font-size:24px;");
    zMValue->setStyleSheet("color:blue;font-size:24px;");
    xLabel->setMaximumWidth(85);
    yLabel->setMaximumWidth(85);
    cLabel->setMaximumWidth(85);
    wLabel->setMaximumWidth(85);
    aLabel->setMaximumWidth(85);
    bLabel->setMaximumWidth(85);
    zLabel->setMaximumWidth(85);
}

//void CoordWidget::setLabels()
//{
//    QSettings* setting = new QSettings("./db/edmsystem.ini",QSettings::IniFormat);
//    setting->beginGroup("systemSet");
//    int xLabelFlag,yLabelFlag,aLabelFlag,bLabelFlag=1;
//    xLabelFlag = setting->value("xLabelFlag").toInt();
//    yLabelFlag = setting->value("yLabelFlag").toInt();
//    aLabelFlag = setting->value("aLabelFlag").toInt();
//    bLabelFlag = setting->value("bLabelFlag").toInt();
//    qDebug()<<xLabelFlag<<yLabelFlag<<aLabelFlag<<bLabelFlag;
//    float zero = 0.000;
//    int count = 0;
//    QString tmp = float2QString(zero);
//    QString xstyle = "font-size:40px;";
//    QString wstyle = "font-size:28px;";
//    if(xLabelFlag == 1)
//    {
//        xLabel = new QLabel("X");
//        labels.append(xLabel);
//    }else{labels.append(nullptr);}
//    if(yLabelFlag == 1)
//    {
//        yLabel = new QLabel("Y");
//        labels.append(yLabel);
//    }else{labels.append(nullptr);}
//    cLabel = new QLabel("C");
//    wLabel = new QLabel("W");
//    labels.append(cLabel);
//    labels.append(wLabel);
//    if(aLabelFlag == 1)
//    {
//        aLabel = new QLabel("A");
//        labels.append(aLabel);

//    }else{labels.append(nullptr);}
//    if(bLabelFlag == 1)
//    {
//        bLabel = new QLabel("B");
//        labels.append(bLabel);

//    }else{labels.append(nullptr);}
//    zLabel = new QLabel("Z");
//    labels.append(zLabel);

//    xValue = new QLabel(tmp);
//    xMValue = new QLabel(tmp);
//    xShow = new QLabel();
//    yValue = new QLabel(tmp);
//    yMValue = new QLabel(tmp);
//    yShow = new QLabel();
//    aValue = new QLabel(tmp);
//    aMValue = new QLabel(tmp);
//    aShow = new QLabel();
//    bValue = new QLabel(tmp);
//    bMValue = new QLabel(tmp);
//    bShow = new QLabel();

//    cValue = new QLabel(tmp);
//    wValue = new QLabel(tmp);
//    zValue = new QLabel(tmp);

//    cMValue = new QLabel(tmp);
//    wMValue = new QLabel(tmp);
//    zMValue = new QLabel(tmp);

//    cShow = new QLabel();
//    wShow = new QLabel();
//    zShow = new QLabel();
//    leftLayout = new QGridLayout(this);
//    count = 0;
//    for(int i =0;i<MAC_LABEL_COUNT;i++)
//    {
//        if(labels[i])
//        {
//            qDebug()<<labels[i];
//            labels[i]->setAlignment(Qt::AlignVCenter);
//            Values[i]->setAlignment(Qt::AlignRight);
//            mValues[i]->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//            leftLayout->addWidget(labels[i],count,0);
//            leftLayout->addWidget(Values[i],count,1);
//            leftLayout->addWidget(mValues[i],count,2);
//            leftLayout->addWidget(shows[i],count,3);
//            if(i==3)//w
//            {
//                labels[i]->setStyleSheet(wstyle);

//            }else{
//                labels[i]->setStyleSheet(xstyle);
//            }
//            Values[i]->setStyleSheet(xstyle);
//            mValues[i]->setStyleSheet("color:blue;font-size:24px;");
//            labels[i]->setMaximumWidth(85);
//            count++;
//        }
//    }
//}

QString CoordWidget::float2QString(float value)
{
    QString str;
    str = QString("%1").arg(value,8,'f',3);
    return str;
}

CoordWidget* CoordWidget::getInstance()
{
    if (!m_coordWid)
        m_coordWid = new CoordWidget();
    return m_coordWid;
}


void CoordWidget::ShowAxisData()
{
    ShowData(edm->m_stEdmShowData.stComm,edm->m_stEdmShowData.iWorkPos);
}


void CoordWidget::ShowData(const MAC_COMMON& stMaccomm,int iRelLabel[])
{
    static MAC_COMMON stMaccomLast;
    static int iRelLabelLast[MAC_LABEL_COUNT] = {0};
    float fTmp;
    QString str;
    memset(&stMaccomLast,0,sizeof(MAC_COMMON));
    for (int i=0;i<MAC_LABEL_COUNT;i++)
    {
        if(labels[i] == nullptr)continue;
        if (stMaccomm.stMoveCtrlComm[i].iMachPos != stMaccomLast.stMoveCtrlComm[i].iMachPos
            || stMaccomm.stMoveCtrlComm[i].iRasilPos != stMaccomLast.stMoveCtrlComm[i].iRasilPos)
        {

            stMaccomLast.stMoveCtrlComm[i].iMachPos = stMaccomm.stMoveCtrlComm[i].iMachPos;
            stMaccomLast.stMoveCtrlComm[i].iRasilPos = stMaccomm.stMoveCtrlComm[i].iRasilPos;
            iRelLabelLast[i]= iRelLabel[i];
            fTmp = (float)stMaccomm.stMoveCtrlComm[i].iMachPos/1000.0;//机械坐标
            if(i == 0&&fTmp<2)qDebug()<<fTmp;
            str = float2QString(fTmp);
            mValues[i]->setText(str);
            fTmp = (float)stMaccomm.stMoveCtrlComm[i].iRasilPos/1000.0;//相对坐标
            str = float2QString(fTmp);
            Values[i]->setText(str);
        }
    }
}

void CoordWidget::SaveData()
{
    static MAC_COMMON stMaccomLast;
    static int iSaveCnt = 0;
    unsigned char bSave = FALSE;
    memset(&stMaccomLast,0,sizeof(MAC_COMMON));
    if (++iSaveCnt>=5)
    {
        //数据写入
        iSaveCnt = 0;
        for (int i=0;i<MAC_LABEL_COUNT;i++)
        {
            if(labels[i] == nullptr)continue;
            if ( edm->m_stEdmShowData.stComm.stMoveCtrlComm[i].iMachPos != stMaccomLast.stMoveCtrlComm[i].iMachPos
                || edm->m_stEdmShowData.stComm.stMoveCtrlComm[i].bDirMove != stMaccomLast.stMoveCtrlComm[i].bDirMove
                || edm->m_stEdmShowData.stComm.stMoveCtrlComm[i].iWorkPosSet != stMaccomLast.stMoveCtrlComm[i].iWorkPosSet)
            {
                bSave = TRUE;
                break;
            }
        }
        if (bSave)
        {
            memcpy(&stMaccomLast,&edm->m_stEdmShowData.stComm,sizeof(MAC_COMMON));
            edm->EdmSaveMacComm();
        }
        bSave = FALSE;
    }
}

void CoordWidget::ShowMacUserStatus()
{
    static unsigned long bPosLimit[MAC_LABEL_COUNT] = {0};
    static unsigned long bNegLimit[MAC_LABEL_COUNT] = {0};
    static unsigned long bAlarm[MAC_LABEL_COUNT] = {0};
    for (int i=0;i<MAC_LABEL_COUNT;i++)
    {
        if(labels[i] == nullptr)continue;
        if (edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bPosLimit != bPosLimit[i])
        {
            bPosLimit[i] = edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bPosLimit;
            if(bPosLimit[i])
            {
                shows[i]->setStyleSheet("border-image:url(zx.png);");
            }else{

                shows[i]->setStyleSheet("");
            }
        }
        //负限
        if (edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bNegLimit!=bNegLimit[i])
        {
            bNegLimit[i] = edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bNegLimit;
            if(bNegLimit[i])
            {
                shows[i]->setStyleSheet("border-image:url(fx.png);");
            }else{

                shows[i]->setStyleSheet("");
            }
        }
        //轴报警

        if (edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bAlarm!=bAlarm[i])
        {
            bAlarm[i] = edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bAlarm;
            if(bAlarm[i])
            {
                if(i==3)//w轴区别对待
                {
                    labels[i]->setStyleSheet("font-size:28px;border-image:url(bj.png);");
                }else{
                    labels[i]->setStyleSheet("font-size:40px;border-image:url(bj.png);");
                }

            }else{
                if(i==3)//w轴区别对待
                {
                    labels[i]->setStyleSheet("font-size:28px");
                }else{
                    labels[i]->setStyleSheet("font-size:40px");
                }
            }
        }
    }
}

void CoordWidget::setAxisValue(int label,QString str)
{
    if(!labels[label])return;
    Values[label]->setText(str);
}

void CoordWidget::HandleEdmCycleData()
{
    edm->GetEdmComm();
    edm->GetEdmStatusData();
    SaveData();              //存储数据
    ShowAxisData();          //显示数据
    ShowMacUserStatus();	 //状态显示
    edm->EdmAxisAdjust();
    edm->EdmAxisAdjustCircle();
}

