#include "coordwidget.h"
#include <QDebug>
#include <QPixmap>
#include <QSettings>

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
            fTmp = (float)edm->m_stEdmShowData.iWorkPos[i]/1000;//相对坐标
            str = float2QString(fTmp);
            m_labels[i+7]->setText(str);
            fTmp = (float)edm->m_stEdmShowData.stComm.stMoveCtrlComm[i].iMachPos/1000;//机械坐标
            str = float2QString(fTmp);
            m_labels[i+14]->setText(str);
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

//显示轴的位置
void CoordWidget::ShowAxisData()
{
    static int iMachPos[MAC_LABEL_COUNT] = {0};
    static int iLabelValue[MAC_LABEL_COUNT] = {0};
    float fTmp;
    QString str;
    for (int i=0;i<MAC_LABEL_COUNT;i++)
    {
        if(!m_labels[i])continue;
        if (edm->m_stEdmShowData.stComm.stMoveCtrlComm[i].iMachPos != iMachPos[i]|| edm->m_stEdmShowData.iWorkPos[i] != iLabelValue[i])
        {
            iLabelValue[i] = edm->m_stEdmShowData.iWorkPos[i];
            iMachPos[i] = edm->m_stEdmShowData.stComm.stMoveCtrlComm[i].iMachPos;
            fTmp = (float)iLabelValue[i]/1000;//相对坐标
            str = float2QString(fTmp);
            m_labels[i+7]->setText(str);
            fTmp = (float)iMachPos[i]/1000;//机械坐标
            str = float2QString(fTmp);
            m_labels[i+14]->setText(str);
        }
    }
}

//显示告警状态
void CoordWidget::ShowMacUserStatus()
{
    static unsigned long bPosLimit[MAC_LABEL_COUNT] = {0};
    static unsigned long bNegLimit[MAC_LABEL_COUNT] = {0};
    static unsigned long bAlarm[MAC_LABEL_COUNT] = {0};
    for (int i=0;i<MAC_LABEL_COUNT;i++)
    {
        if(!m_labels[i] || i == 6)continue;//z轴没有告警
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

void CoordWidget::HandleEdmCycleData()
{
    ShowAxisData();          //显示数据
    ShowMacUserStatus();	 //状态显示
}

