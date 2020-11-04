#ifndef INITDB_H
#define INITDB_H

#include <QtSql>
#include <QMessageBox>

void addVal(QSqlQuery &q, int iVal)
{
    q.addBindValue(iVal);
    q.exec();
}

QSqlError initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sino.db");

    if (!db.open()) {
        QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
            QObject::tr("Unable to establish a database connection.\n"
                        "This example needs SQLite support. Please read "
                        "the Qt SQL driver documentation for information how "
                        "to build it.\n\n"
                        "Click Cancel to exit."), QMessageBox::Cancel);
        return db.lastError();
    }

//    QStringList tables = db.tables();
//    if (tables.contains("x_label", Qt::CaseInsensitive))
//        return QSqlError();

    QSqlQuery q;
    q.exec(QLatin1String("create table x_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table label(iIndex integer primary key, cSymbol varchar,bUse integer,bRotateLabel integer,iChibu integer,bPosMonitor integer,fRasterLinear integer,iPosPrecision integer,bLimitNoUse Integer,iZeroMakeUp integer,iLowerPos integer,iTopPos integer,iLocateArea integer,bDirectMotor integer)"));
    q.exec(QLatin1String("create table label_var(iIndex integer primary key, iMachPos integer,bDir integer,iRelSet_0 integer,iRelSet_1 integer,iRelSet_2 integer,iRelSet_3 integer,iRelSet_4 integer,iRelSet_5 integer)"));
    q.exec(QLatin1String("create table total(iIndex integer primary key, filename varchar,workindex integer,prune varchar,total varchar,iAxisLabel integer)"));
    q.exec(QLatin1String("create table elec_oral (\
                         id int primary key,\
                         iTimeMin int,         \
                         iTimeMax int,           \
                         iRisePos int,          \
                         iSafePos int,          \
                         iJudgePos int,         \
                         iRepeatCount int,      \
                         iRepeatLen int,        \
                         iBottomSleep int,     \
                         iMillServo int,     \
                         iOpLenAll int,      \
                         iOpHoleIndex int,    \
                         iOpHoleAll int,       \
                         bRotateValidate int,  \
                         bContinueOp int,       \
                         filename varchar(40))"));
    q.exec(QLatin1String("create table elec_page(\
                         id int primary key,\
                         iTon int,          \
                         iToff int,         \
                         iElecLow int,      \
                         iElecHigh int,     \
                         iServo int,        \
                         iFeedSense int,   \
                         iBackSense int,    \
                         iOpLen int,        \
                         iCap int,          \
                         iRotSpeed int,    \
                         elecid int, \
                         foreign key(elecid) references elec_oral(id)"));
    
    return QSqlError();
}

#endif
