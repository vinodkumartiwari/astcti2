#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>
#include <QSqlQuery>

class Database {
public:
    static QSqlDatabase connect();
    static QSqlDatabase connectRoot(QString rootPw);
    static void closeDb();


};

#endif        //  #ifndef DATABASE_H
