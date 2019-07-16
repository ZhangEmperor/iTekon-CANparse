#ifndef ATDBC_FILE2DB_H
#define ATDBC_FILE2DB_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>

#include "dbcmodel.h"
#include "dbcreader.h"

//将dbc文件解析到数据库中

class ATdbc_file2db
{
public:
    ATdbc_file2db(QString dbcfile);

    ~ATdbc_file2db();
private:

    QSqlDatabase db_handle;
    int ATdbc_dbinit();
    int ATdbc_filehandle(QString filename);
    void show_network(dbc_t *dbc);
    void show_message_list(message_list_t *message_list);
    void show_message(QSqlQuery &sqlexec, message_list_t *ml);
    void show_signals(dbc_t *dbc);
    void show_signal(QSqlQuery &sqlexec, signal_list_t *sl);
    void show_nodes(dbc_t *dbc);
    void show_envvars(envvar_list_t *envvar_list);
    void show_valtable_list(valtable_list_t *valtable_list);

    QString ATdbc_attributelist_to_qstring(attribute_list_t *al);
    QString ATdbc_stringlist_to_qstring(string_list_t *string_list);
    QString Atdbc_valmap_to_qstring(val_map_t *vm);
};

#endif // ATDBC_FILE2DB_H
