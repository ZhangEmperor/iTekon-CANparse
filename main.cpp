#include "mainwindow.h"
#include <QApplication>

#include "atdbc_file2db.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    ATdbc_file2db *dbchandle = new ATdbc_file2db("/home/zap/workspace/candbc/J1939_bms.dbc");

//    delete dbchandle;

    return a.exec();
}
