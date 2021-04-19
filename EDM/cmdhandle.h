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

private:
    void AnalyseCmdMode(QString &str,DIGIT_CMD *pDigitCmd);
    void AnalyseCommand(QString strCommand,DIGIT_CMD *pDigitCmd,DIGIT_CMD* cmdDefault);
    void WriteCmd2Para(QString strCmd,DIGIT_CMD *pDigitCmd);
    int FindFirstLabel(QString strCmd);
    int  GetLableIndex(char cComLabel);

    int FindAndCalSpeed(QString strCmd);
    int FindAndCalDepth(QString strCmd,short& dwDepth);
    void QStringtoFloat(QString str,float &fFloat);
    int CheckCmdFig(QString strCmd,bool* pLaw);
    void AnalyseLastWord(QString &str);
    unsigned char HasDot(QString strCmd);

    int GetAntanAngle(unsigned char bFourQuant,float fSon,float fMon);

    int GetALabel_xDis(int iLabel_A,int iRadius,unsigned char bAntiClockWise);
    int GetALabel_zDis(int iLabel_A,int iRadius,unsigned char bAntiClockWise);
    int GetCLabel_xDis(int iLabel_C,int iRadius,unsigned char bAntiClockWise);
    int GetCLabel_yDis(int iLabel_C,int iRadius,unsigned char bAntiClockWise);
};

#endif // CMDHANDLE_H
