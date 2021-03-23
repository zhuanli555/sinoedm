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
//void CoordWidget::setLabels()
//{
//    xLabel = new QLabel("X");
//    yLabel = new QLabel("Y");
//    cLabel = new QLabel("C");
//    wLabel = new QLabel("W");
//    aLabel = new QLabel("A");
//    bLabel = new QLabel("B");
//    zLabel = new QLabel("Z");
//    //test
//    QLabel* test = new QLabel;
//    m_labels<<xLabel<<yLabel<<cLabel<<wLabel<<aLabel<<bLabel<<zLabel<<test;
//    float zero = 0.000;
//    QString tmp1 = QString("%1").arg("-123.567");
//    QString tmp2 = QString("%1").arg(zero,8,'f',3);
//    xValue = new QLabel(tmp1);
//    yValue = new QLabel(tmp2);
//    cValue = new QLabel(tmp2);
//    zValue = new QLabel(tmp2);
//    xMValue = new QLabel(tmp1);
//    yMValue = new QLabel(tmp2);
//    cMValue = new QLabel(tmp2);
//    zMValue = new QLabel(tmp2);

//    wValue = new QLabel(tmp1);
//    aValue = new QLabel(tmp2);
//    bValue = new QLabel(tmp2);
//    wMValue = new QLabel(tmp1);
//    aMValue = new QLabel(tmp2);
//    bMValue = new QLabel(tmp2);

//    xShow = new QLabel();
//    yShow = new QLabel();
//    cShow = new QLabel();
//    wShow = new QLabel();
//    aShow = new QLabel();
//    bShow = new QLabel();
//    zShow = new QLabel();

//    xLabel->setAlignment(Qt::AlignVCenter);
//    yLabel->setAlignment(Qt::AlignVCenter);
//    cLabel->setAlignment(Qt::AlignVCenter);
//    wLabel->setAlignment(Qt::AlignVCenter);
//    aLabel->setAlignment(Qt::AlignVCenter);
//    bLabel->setAlignment(Qt::AlignVCenter);
//    zLabel->setAlignment(Qt::AlignVCenter);
//    xValue->setAlignment(Qt::AlignRight);
//    yValue->setAlignment(Qt::AlignRight);
//    cValue->setAlignment(Qt::AlignRight);
//    wValue->setAlignment(Qt::AlignRight);
//    aValue->setAlignment(Qt::AlignRight);
//    bValue->setAlignment(Qt::AlignRight);
//    zValue->setAlignment(Qt::AlignRight);
//    xMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//    yMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//    cMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//    wMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//    aMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//    bMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
//    zMValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

//    leftLayout = new QGridLayout(this);
//    leftLayout->addWidget(xLabel,0,0);
//    leftLayout->addWidget(xValue,0,1);
//    leftLayout->addWidget(xMValue,0,2);leftLayout->addWidget(xShow,0,3);
//    leftLayout->addWidget(yLabel,1,0);
//    leftLayout->addWidget(yValue,1,1);
//    leftLayout->addWidget(yMValue,1,2);leftLayout->addWidget(yShow,1,3);
//    leftLayout->addWidget(cLabel,2,0);
//    leftLayout->addWidget(cValue,2,1);
//    leftLayout->addWidget(cMValue,2,2);leftLayout->addWidget(cShow,2,3);
//    leftLayout->addWidget(wLabel,3,0);
//    leftLayout->addWidget(wValue,3,1);
//    leftLayout->addWidget(wMValue,3,2);leftLayout->addWidget(wShow,3,3);
//    leftLayout->addWidget(aLabel,4,0);
//    leftLayout->addWidget(aValue,4,1);
//    leftLayout->addWidget(aMValue,4,2);leftLayout->addWidget(aShow,4,3);
//    leftLayout->addWidget(bLabel,5,0);
//    leftLayout->addWidget(bValue,5,1);
//    leftLayout->addWidget(bMValue,5,2);leftLayout->addWidget(bShow,5,3);
//    leftLayout->addWidget(zLabel,6,0);
//    leftLayout->addWidget(zValue,6,1);
//    leftLayout->addWidget(zMValue,6,2);leftLayout->addWidget(zShow,6,3);
//    //setStyleSheet("QLabel{background-color:yellow}");
//    QString xstyle = "font-size:40px;";
//    QString wstyle = "font-size:28px;";
//    //bShow->setStyleSheet("border-image:url(icon/zx.png);")
//    xLabel->setStyleSheet(xstyle);
//    yLabel->setStyleSheet(xstyle);
//    cLabel->setStyleSheet(xstyle);
//    wLabel->setStyleSheet(wstyle);
//    aLabel->setStyleSheet(xstyle);
//    bLabel->setStyleSheet(xstyle);
//    zLabel->setStyleSheet(xstyle);
//    xValue->setStyleSheet(xstyle);
//    yValue->setStyleSheet(xstyle);
//    cValue->setStyleSheet(xstyle);
//    wValue->setStyleSheet(xstyle);
//    aValue->setStyleSheet(xstyle);
//    bValue->setStyleSheet(xstyle);
//    zValue->setStyleSheet(xstyle);
//    xMValue->setStyleSheet("color:blue;font-size:24px;");
//    yMValue->setStyleSheet("color:blue;font-size:24px;");
//    cMValue->setStyleSheet("color:blue;font-size:24px;");
//    wMValue->setStyleSheet("color:blue;font-size:24px;");
//    aMValue->setStyleSheet("color:blue;font-size:24px;");
//    bMValue->setStyleSheet("color:blue;font-size:24px;");
//    zMValue->setStyleSheet("color:blue;font-size:24px;");
//    xLabel->setMaximumWidth(85);
//    yLabel->setMaximumWidth(85);
//    cLabel->setMaximumWidth(85);
//    wLabel->setMaximumWidth(85);
//    aLabel->setMaximumWidth(85);
//    bLabel->setMaximumWidth(85);
//    zLabel->setMaximumWidth(85);
//}

void CoordWidget::setLabels()
{
    QSettings* setting = new QSettings("./db/edmsystem.ini",QSettings::IniFormat);
    setting->beginGroup("systemSet");

    int xLabelFlag,yLabelFlag,aLabelFlag,bLabelFlag=1;
    xLabelFlag = setting->value("xLabelFlag").toInt();
    yLabelFlag = setting->value("yLabelFlag").toInt();
    aLabelFlag = setting->value("aLabelFlag").toInt();
    bLabelFlag = setting->value("bLabelFlag").toInt();
    float zero = 0.000;
    QLabel* null = nullptr;
    QString tmp = float2QString(zero);
    QString xstyle = "font-size:40px;";
    QString wstyle = "font-size:28px;";
    if(xLabelFlag == 1)
    {
        QLabel* xLabel = new QLabel("X");
        m_labels.append(xLabel);
    }else{m_labels.append(null);}
    if(yLabelFlag == 1)
    {
        QLabel* yLabel = new QLabel("Y");
        m_labels.append(yLabel);
    }else{m_labels.append(null);}
    QLabel* cLabel = new QLabel("C");
    QLabel* wLabel = new QLabel("W");
    m_labels.append(cLabel);
    m_labels.append(wLabel);
    if(aLabelFlag == 1)
    {
        QLabel* aLabel = new QLabel("A");
        m_labels.append(aLabel);

    }else{m_labels.append(null);}
    if(bLabelFlag == 1)
    {
        QLabel* bLabel = new QLabel("B");
        m_labels.append(bLabel);

    }else{m_labels.append(null);}
    QLabel* zLabel = new QLabel("Z");
    m_labels.append(zLabel);

    QLabel* xValue = new QLabel(tmp);
    QLabel* xMValue = new QLabel(tmp);
    QLabel* xShow = new QLabel;
    QLabel* yValue = new QLabel(tmp);
    QLabel* yMValue = new QLabel(tmp);
    QLabel* yShow = new QLabel;
    QLabel* aValue = new QLabel(tmp);
    QLabel* aMValue = new QLabel(tmp);
    QLabel* aShow = new QLabel;
    QLabel* bValue = new QLabel(tmp);
    QLabel* bMValue = new QLabel(tmp);
    QLabel* bShow = new QLabel;

    QLabel* cValue = new QLabel(tmp);
    QLabel* wValue = new QLabel(tmp);
    QLabel* zValue = new QLabel(tmp);

    QLabel* cMValue = new QLabel(tmp);
    QLabel* wMValue = new QLabel(tmp);
    QLabel* zMValue = new QLabel(tmp);

    QLabel* cShow = new QLabel;
    QLabel* wShow = new QLabel;
    QLabel* zShow = new QLabel;
    m_labels<<xValue<<yValue<<cValue<<wValue<<aValue<<bValue<<zValue;
    m_labels<<xMValue<<yMValue<<cMValue<<wMValue<<aMValue<<bMValue<<zMValue;
    m_labels<<xShow<<yShow<<cShow<<wShow<<aShow<<bShow<<zShow;
    QGridLayout* leftLayout = new QGridLayout(this);
    int i,count = 0;
    float fTmp;
    QString str;
    for (i=0;i<MAC_LABEL_COUNT;i++)
    {
        if(m_labels[i])
        {
            fTmp = (float)edm->m_stEdmShowData.stComm.stMoveCtrlComm[i].iMachPos/1000.0;//机械坐标
            str = float2QString(fTmp);
            m_labels[i+14]->setText(str);
            fTmp = (float)edm->m_stEdmShowData.stComm.stMoveCtrlComm[i].iRasilPos/1000.0;//相对坐标
            str = float2QString(fTmp);
            m_labels[i+7]->setText(str);
        }
    }
    for(i =0;i<MAC_LABEL_COUNT;i++)
    {
        if(m_labels[i])
        {
            m_labels[i]->setAlignment(Qt::AlignVCenter);
            m_labels[i+7]->setAlignment(Qt::AlignRight);
            m_labels[i+14]->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
            leftLayout->addWidget(m_labels[i],count,0);
            leftLayout->addWidget(m_labels[i+7],count,1);
            leftLayout->addWidget(m_labels[i+14],count,2);
            leftLayout->addWidget(m_labels[i+21],count,3);
            if(i==3)//w
            {
                m_labels[i]->setStyleSheet(wstyle);

            }else{
                m_labels[i]->setStyleSheet(xstyle);
            }
            m_labels[i+7]->setStyleSheet(xstyle);
            m_labels[i+14]->setStyleSheet("color:blue;font-size:24px;");
            m_labels[i]->setMaximumWidth(85);
            count++;
        }
    }
}

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
        if(!m_labels[i])continue;
        if (stMaccomm.stMoveCtrlComm[i].iMachPos != stMaccomLast.stMoveCtrlComm[i].iMachPos
            || stMaccomm.stMoveCtrlComm[i].iRasilPos != stMaccomLast.stMoveCtrlComm[i].iRasilPos)
        {

            stMaccomLast.stMoveCtrlComm[i].iMachPos = stMaccomm.stMoveCtrlComm[i].iMachPos;
            stMaccomLast.stMoveCtrlComm[i].iRasilPos = stMaccomm.stMoveCtrlComm[i].iRasilPos;
            iRelLabelLast[i]= iRelLabel[i];
            fTmp = (float)stMaccomm.stMoveCtrlComm[i].iMachPos/1000.0;//机械坐标
            if(i == 0&&fTmp<2)qDebug()<<fTmp;
            str = float2QString(fTmp);
            m_labels[i+14]->setText(str);
            fTmp = (float)stMaccomm.stMoveCtrlComm[i].iRasilPos/1000.0;//相对坐标
            str = float2QString(fTmp);
            m_labels[i+7]->setText(str);
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
        if(!m_labels[i])continue;
        if (edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bPosLimit != bPosLimit[i])
        {
            bPosLimit[i] = edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bPosLimit;
            if(bPosLimit[i])
            {
                m_labels[i+21]->setStyleSheet("border-image:url(icon/zx.png);");
            }else{

                m_labels[i+21]->setStyleSheet("");
            }
        }
        //负限
        if (edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bNegLimit!=bNegLimit[i])
        {
            bNegLimit[i] = edm->m_stEdmShowData.stHardCtl.stHardCtlUser[i].bNegLimit;
            if(bNegLimit[i])
            {
                m_labels[i+21]->setStyleSheet("border-image:url(icon/fx.png);");
            }else{

                m_labels[i+21]->setStyleSheet("");
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
                    m_labels[i]->setStyleSheet("font-size:28px;border-image:url(icon/bj.png);");
                }else{
                    m_labels[i]->setStyleSheet("font-size:40px;border-image:url(icon/bj.png);");
                }

            }else{
                if(i==3)//w轴区别对待
                {
                    m_labels[i]->setStyleSheet("font-size:28px");
                }else{
                    m_labels[i]->setStyleSheet("font-size:40px");
                }
            }
        }
    }
}

void CoordWidget::setAxisValue(int label,QString str)
{
    if(!m_labels[label])return;
    m_labels[label+7]->setText(str);
}

void CoordWidget::HandleEdmCycleData()
{
    SaveData();              //存储数据
    ShowAxisData();          //显示数据
    ShowMacUserStatus();	 //状态显示
}

