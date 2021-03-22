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

   QStringList tables = db.tables();
   if (tables.contains("elec", Qt::CaseInsensitive))
       return QSqlError();

    QSqlQuery q;
    q.exec(QLatin1String("create table x_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table y_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table z_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table a_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table b_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table c_label(iIndex integer primary key, iPos integer,iVal integer)"));
    q.exec(QLatin1String("create table label(iIndex integer primary key, cSymbol varchar(4),bUse integer,bRotateLabel integer,iChibu integer,bPosMonitor integer,fRasterLinear integer,iPosPrecision integer,bLimitNoUse Integer,iZeroMakeUp integer,iLowerPos integer,iTopPos integer,iLocateArea integer,bDirectMotor integer)"));
    q.exec(QLatin1String("create table label_var(iIndex integer primary key, iMachPos integer,bDir integer,iRelSet_0 integer,iRelSet_1 integer,iRelSet_2 integer,iRelSet_3 integer,iRelSet_4 integer,iRelSet_5 integer)"));
    q.exec(QLatin1String("create table total(iIndex integer primary key, filename varchar(32),workindex integer,prune varchar(128),total varchar(255),iAxisLabel integer)"));
    q.exec(QLatin1String("create table elec(iIndex integer primary key,eName varchar(24),elec_page_0 varchar(64),elec_page_1 varchar(64),elec_page_2 varchar(64),elec_page_3 varchar(64),elec_page_4 varchar(64),elec_page_5 varchar(64),elec_oral varchar(128))"));
    
    return QSqlError();
}
#endif
