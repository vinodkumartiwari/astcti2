#include "database.h"
#include <QDebug>
//start id="connect"
QSqlDatabase Database::connect() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("cti");
    db.setUserName("root");
    db.setPassword("rundllbrsoft");
    if (!db.open()) qDebug() << "Failed to connect to mysql as user cti" ;
    return db;
}
//end
//start id=connectroot
QSqlDatabase Database::connectRoot(QString rootPw) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("mysql");
    db.setUserName("root");
    db.setPassword(rootPw);
    if (!db.open()) qDebug() << "Failed to connect to root mysql admin";
    return db;
}
//end
void Database::closeDb() {
    QSqlDatabase::removeDatabase("QMYSQL");
}
