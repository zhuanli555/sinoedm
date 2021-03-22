#ifndef CMDHANDLE_H
#define CMDHANDLE_H

#include "EDM.h"
#include <vector>

class CmdHandle
{
public:
    CmdHandle(unsigned char bVectCmd,QString strCommand,DIGIT_CMD *pDigitCmd,DIGIT_CMD* cmdDefault);
    ~CmdHandle();

    static void DigitCmd2QString(DIGIT_CMD *pCmd,QString& strCmd);
    static int  GetSpeedFreq(int iSpeed);
    static int GetSpeed(int iFreq);
    static int GetRAxisFreq(int iSpeed);
    //from ccmdhandle

    static QString GetElecPagePara2QString(Elec_Page* pElecPage);
    static QString GetElecOralPara2QString(Elec_Oral* pElecOral);
    static QString GetElecPara2QString(QString strName,MAC_ELEC_PARA* pElecPara);
    static QString GetElecPageParaFromQString(QString strCmd,Elec_Page* pElecPage);
    static QString GetElecOralParaFromQString(QString strCmd,Elec_Oral* pElecOral);
    static unsigned char ReadCmdFromFile(QString strPath,QString strFile,vector<QString>* pVector,MAP_ELEC_MAN* pMap);

    static void LawInt(int& t,int low,int high);
    static int  PercentStr2int(QString str);
    static void SetCmdCount(int iCount);
    static void LawOfPara(MAC_ELEC_PARA* pPara);

private:
    void AnalyseCmdMode(QString &str,DIGIT_CMD *pDigitCmd);
    void AnalyseCommand(QString strCommand,DIGIT_CMD *pDigitCmd,DIGIT_CMD* cmdDefault);
    void WriteCmd2Para(QString strCmd,DIGIT_CMD *pDigitCmd);
    int FindFirstLabel(QString strCmd);
    int  GetLableIndex(char cComLabel);

    void AnalyseVectCommand(QString strCommand,DIGIT_CMD *pDigitCmd,DIGIT_CMD* cmdDefault);
    void WriteVectCmd2Para(QString strCmd,DIGIT_CMD_VECT *pDigitCmd);
    int FindFirstLabelVect(QString strCmd);
    int  GetLableIndexVect(char cComLabel);
    void Vect2DigitCmd(DIGIT_CMD_VECT* pVectCmd,DIGIT_CMD* pDigitCmd);

    int FindAndCalSpeed(QString strCmd);
    int FindAndCalDepth(QString strCmd,short& dwDepth);
    void QStringtoFloat(QString str,float &fFloat);
    int CheckCmdFig(QString strCmd,bool* pLaw);
    void AnalyseLastWord(QString &str);
    unsigned char HasDot(QString strCmd);

    int GetAntanAngle(unsigned char bFourQuant,float fSon,float fMon);

    void VectSort(DIGIT_CMD_VECT* pVectCmd);

    int GetALabel_xDis(int iLabel_A,int iRadius,unsigned char bAntiClockWise);
    int GetALabel_zDis(int iLabel_A,int iRadius,unsigned char bAntiClockWise);
    int GetCLabel_xDis(int iLabel_C,int iRadius,unsigned char bAntiClockWise);
    int GetCLabel_yDis(int iLabel_C,int iRadius,unsigned char bAntiClockWise);
};

#endif // CMDHANDLE_H
