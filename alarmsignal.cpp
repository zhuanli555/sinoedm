#include "alarmsignal.h"
#include <QTextCodec>

AlarmSignal* AlarmSignal::m_alarmSig = nullptr;
AlarmSignal::AlarmSignal(QWidget *parent) : QWidget(parent)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    setMaximumWidth(600);
    edm = EDM::GetEdmInstance();
    edmOpList = EDM_OP_List::GetEdmOpListPtr();
    bPause = false;
    memset(&m_stEntileStatus,0,sizeof(EDM_SHOW_STATUS));
    purgeValue = new QLabel(QString::fromLocal8Bit("��Һ(F4)"));
    lowPumpValue = new QLabel(QString::fromLocal8Bit("��ѹ��(F5)"));
    shakeValue = new QLabel(QString::fromLocal8Bit("��(F6)"));
    protectValue = new QLabel(QString::fromLocal8Bit("��ײ����(F7)"));
    pauseValue = new QLabel(QString::fromLocal8Bit("��ͣ(F8)"));
    findCenter = new QLabel(QString::fromLocal8Bit("������(F)"));
    mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->addWidget(purgeValue,0,0);
    mainLayout->addWidget(lowPumpValue,1,0);
    mainLayout->addWidget(shakeValue,2,0);
    mainLayout->addWidget(protectValue,3,0);
    mainLayout->addWidget(pauseValue,4,0);
    mainLayout->addWidget(findCenter,5,0);
    findCenter->setMaximumWidth(85);
    purgeValue->setStyleSheet("background-color:green;");
    lowPumpValue->setStyleSheet("background-color:green;");
    shakeValue->setStyleSheet("background-color:green;");
    protectValue->setStyleSheet("background-color:green;");
    pauseValue->setStyleSheet("background-color:green;");

}

AlarmSignal* AlarmSignal::getInstance()
{
    if (!m_alarmSig)
        m_alarmSig = new AlarmSignal();
    return m_alarmSig;
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
    static unsigned long bDirect = FALSE;
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

    if (bDirect != edm->m_stEdmShowData.stStatus.bDirect)
    {
        bDirect = edm->m_stEdmShowData.stStatus.bDirect;
        edm->EdmHummer(bDirect);
    }
}

//�����ӹ�״̬
//void AlarmSignal::HandleEdmOpStatus()
//{
//    static int iCmdIndex=-1;
//    static unsigned char bOver=FALSE;
//    static OP_ERROR op_error = OP_NO_ERR;
//    static vector<QString> vCmd;
//    static MAP_ELEC_MAN mpElec;

//    EDM_OP* pOp;

//    if (!edmOpList)
//    {
//        return;
//    }

//    pOp = edmOpList->m_pEdmOp;
//    if (pOp != edmOp || edmOpList->m_bChange)
//    {
//        edmOp = pOp;
//        edmOpList->m_bChange = FALSE;
//    }

//    if (pOp->m_stOpStatus.stCycle.bPauseCmd)
//    {
//        pOp->m_stOpStatus.stCycle.bPauseCmd = FALSE;
//    }

//    if(pOp->m_stOpStatus.iCmdIndex != iCmdIndex && EDM_OP::m_bStartCount)
//    {
//        iCmdIndex = pOp->m_stOpStatus.iCmdIndex;
//    }

//    if (pOp->m_stOpStatus.stCycle.stPassChart.bClear)
//    {
//        pOp->m_stOpStatus.stCycle.stPassChart.bClear = FALSE;
//    }
//    else
//    {
//        if (pOp->m_stOpStatus.stCycle.stPassChart.bSet)
//        {
//            pOp->m_stOpStatus.stCycle.stPassChart.bSet = FALSE;
//        }
//        else
//        {
//            if (pOp->m_stOpStatus.stCycle.stPassChart.bRealTimeIn)
//            {
//                pOp->m_stOpStatus.stCycle.stPassChart.bRealTimeIn = FALSE;
//            }
//        }
//    }


//    if (pOp->m_stOpStatus.enErrAll.errOp != op_error )
//    {
//        op_error = pOp->m_stOpStatus.enErrAll.errOp;
//        edm->EdmYellowLump( !(pOp->m_stOpStatus.enErrAll.errOp==OP_NO_ERR));
//    }

////    m_dlgOpStatus.SetOpStatusPara(pOp->m_stOpStatus.iCmdIndex
////                                  ,pOp->m_stOpStatus.stCycle.iCycleIndex
////                                  ,pOp->m_stOpStatus.stCycle.iTimeSec
////                                  ,pOp->m_stOpStatus.stCycle.iOpPage);//��ת��

//    if (pOp->m_stOpStatus.bCheck_C_Over)
//    {
//        pOp->m_stOpStatus.bCheck_C_Over = FALSE;
//    }

//    if (bOver != edmOpList->m_bOver)
//    {
//        bOver = edmOpList->m_bOver;
//        if (bOver)
//        {
//            iCmdIndex = -1;
//        }
//    }
//}

void AlarmSignal::edmPurge()
{
    edm->EdmLowPump(!edm->m_stEdmShowData.stStatus.bPumpLow);//��ѹ ��Һ��
    if(edm->m_stEdmShowData.stStatus.bPumpLow)
    {
        purgeValue->setStyleSheet("background-color:red;");
    }
    else{
        purgeValue->setStyleSheet("background-color:green;");
    }
}

void AlarmSignal::edmLowerPump()
{
    edm->EdmLowPump(!edm->m_stEdmShowData.stStatus.bPumpLow);//��ѹ
    if(edm->m_stEdmShowData.stStatus.bPumpLow)
    {
        lowPumpValue->setStyleSheet("background-color:red;");
    }
    else{
        lowPumpValue->setStyleSheet("background-color:green;");
    }
}

void AlarmSignal::edmShake()
{
    edm->EdmSetShake(edm->m_stEdmShowData.stStatus.bShake);
    if(edm->m_stEdmShowData.stStatus.bShake)
    {
        shakeValue->setStyleSheet("background-color:green;");
    }else{
        shakeValue->setStyleSheet("background-color:red;");
    }
}

void AlarmSignal::edmHandProcess()
{
    if (!m_stEntileStatus.bOpIn && !m_stEntileStatus.bRTzero)
    {
        edm->EdmHandProcess();
    }
}

void AlarmSignal::edmProtect()
{
    edm->EdmSetProtect(edm->m_stEdmShowData.stStatus.bNoProtect);
    if(edm->m_stEdmShowData.stStatus.bNoProtect)
    {
        protectValue->setStyleSheet("background-color:green;");
    }else{
        protectValue->setStyleSheet("background-color:red;");
    }
}

void AlarmSignal::edmPause()
{
    if (!m_stEntileStatus.bOpIn || m_stEntileStatus.bRTzero)
    {
        return;
    }
    bPause = !bPause;
    if(bPause)
    {
        pauseValue->setStyleSheet("background-color:red;");
        pauseValue->setText(QString::fromLocal8Bit("����(F8)"));
    }else{
        pauseValue->setStyleSheet("background-color:green;");
        pauseValue->setText(QString::fromLocal8Bit("��ͣ(F8)"));
    }
    if (!edmOpList->m_pEdmOp)
        return;
    edmOpList->m_pEdmOp->EdmOpSetStart(bPause);
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

