#include "alarmsignal.h"
#include <QTextCodec>

AlarmSignal::AlarmSignal(QWidget *parent) : QWidget(parent)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    setMaximumWidth(600);
    edm = EDM::GetEdmInstance();
    edmOpList = EDM_OP_List::GetEdmOpListPtr();
    bPause = false;
    memset(&m_stEntileStatus,0,sizeof(EDM_SHOW_STATUS));
    purgeValue = new QLabel(QString::fromLocal8Bit("³åÒº(F4)"));
    powerValue = new QLabel(QString::fromLocal8Bit("¸ßÆµ(F5)"));
    shakeValue = new QLabel(QString::fromLocal8Bit("Õñ¶¯(F6)"));
    protectValue = new QLabel(QString::fromLocal8Bit("·À×²±£»¤(F7)"));
    pauseValue = new QLabel(QString::fromLocal8Bit("ÔÝÍ£(F8)"));
    findCenter = new QLabel(QString::fromLocal8Bit("ÕÒÖÐÐÄ(F)"));
    mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->addWidget(purgeValue,0,0);
    mainLayout->addWidget(powerValue,1,0);
    mainLayout->addWidget(shakeValue,2,0);
    mainLayout->addWidget(protectValue,3,0);
    mainLayout->addWidget(pauseValue,4,0);
    mainLayout->addWidget(findCenter,5,0);
    findCenter->setMaximumWidth(85);
    purgeValue->setStyleSheet("background-color:green;");
    powerValue->setStyleSheet("background-color:green;");
    shakeValue->setStyleSheet("background-color:green;");
    protectValue->setStyleSheet("background-color:green;");
    pauseValue->setStyleSheet("background-color:green;");

}


int AlarmSignal::addAlarm(const QString &text)
{

    QLabel* pLabel = new QLabel(text);
    pLabel->setStyleSheet("background-color:red;");
    pLabel->setMaximumWidth(85);
    alarmList.append(pLabel);
    reSort();
    return alarmList.size()-1;
}

void AlarmSignal::removeAlarm(int index)
{
    alarmList.removeAt(index);
    reSort();
}

void AlarmSignal::reSort()
{
    int i ,j;
    if(alarmList.isEmpty())return;
    for(i = 0; i < alarmList.size();i++)
    {
        j = i/6+1;//column
        mainLayout->addWidget(alarmList.at(i),i%6,j);
    }
}

void AlarmSignal::EdmStatusSignChange()
{
    static Mac_Status status;
    memset(&status,0,sizeof(Mac_Status));
    if (edm->m_stEdmShowData.stStatus.bStop || m_stEntileStatus.bStop)
    {
        if (++m_stEntileStatus.iStopCnt >= 10)
        {
            m_stEntileStatus.iStopCnt = 0;
            m_stEntileStatus.bStop = FALSE;
            edm->EdmStopSignClose();
        }
    }

    if (m_stEntileStatus.bRTzero)
    {
        if (edm->m_stEdmShowData.stComm.enMvStatus == RULE_MOVE_OVER)
        {
            m_stEntileStatus.bRTzero = FALSE;
            edm->EdmZeroSignClose();
        }
    }
    else
    {
        if (edm->m_stEdmShowData.stComm.enMvStatus == RULE_RTZERO)
        {
            m_stEntileStatus.bRTzero = TRUE;
        }
    }

    if (status.bDirect != edm->m_stEdmShowData.stStatus.bDirect)
    {
        status.bDirect = edm->m_stEdmShowData.stStatus.bDirect;
        edm->EdmHummer(status.bDirect);
    }
    if (status.bPower != edm->m_stEdmShowData.stStatus.bPower)
    {
        status.bPower = edm->m_stEdmShowData.stStatus.bPower;
        if(status.bPower)
        {
            powerValue->setStyleSheet("background-color:red;");
        }
        else{
            powerValue->setStyleSheet("background-color:green;");
        }
    }
    if (status.bNoProtect != edm->m_stEdmShowData.stStatus.bNoProtect)
    {
        status.bNoProtect = edm->m_stEdmShowData.stStatus.bNoProtect;
        if(status.bNoProtect)
        {
            protectValue->setStyleSheet("background-color:red;");
        }
        else{
            protectValue->setStyleSheet("background-color:green;");
        }
    }
    if (status.bShake != edm->m_stEdmShowData.stStatus.bShake)
    {
        status.bShake = edm->m_stEdmShowData.stStatus.bShake;
        if(status.bShake)
        {
            shakeValue->setStyleSheet("background-color:red;");
        }
        else{
            shakeValue->setStyleSheet("background-color:green;");
        }
    }
    if (status.bPumpLow != edm->m_stEdmShowData.stStatus.bPumpLow)
    {
        status.bPumpLow = edm->m_stEdmShowData.stStatus.bPumpLow;
        if(status.bPumpLow)
        {
            purgeValue->setStyleSheet("background-color:red;");
        }
        else{
            purgeValue->setStyleSheet("background-color:green;");
        }
    }
}

void AlarmSignal::edmHandProcess()
{
    if (!m_stEntileStatus.bOpIn && !m_stEntileStatus.bRTzero)
    {
        edm->EdmHandProcess();
    }
}

void AlarmSignal::edmPause()
{
    if (!edmOpList->m_pEdmOp)
        return;
    edmOpList->m_pEdmOp->EdmOpSetStart(bPause);
    bPause = !bPause;
    if(bPause)
    {
        pauseValue->setStyleSheet("background-color:red;");
        pauseValue->setText(QString::fromLocal8Bit("¼ÌÐø(F8)"));
    }else{
        pauseValue->setStyleSheet("background-color:green;");
        pauseValue->setText(QString::fromLocal8Bit("ÔÝÍ£(F8)"));
    }  
}

void AlarmSignal::edmStop()
{
    if (m_stEntileStatus.bStop)
            return;
    m_stEntileStatus.bStop = TRUE;
    if (!m_stEntileStatus.bOpIn)
    {
        edm->EdmStop();
    }
    m_stEntileStatus.bOpIn = FALSE;
    edmOpList->EdmOpListOver();
}

