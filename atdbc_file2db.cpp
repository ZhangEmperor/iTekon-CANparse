#include "atdbc_file2db.h"
#include <QDebug>

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <libgen.h>
#include <getopt.h>

QString decode_mux_type(mux_t mux_type)
{
    QString text;

    switch(mux_type) {
    case m_signal:      text = "Signal"; break;
    case m_multiplexor: text = "Multiplexor"; break;
    case m_multiplexed: text = "Multiplexed"; break;
    }

    return text;
}

QString decode_signal_val_type(signal_val_type_t signal_val_type)
{
    QString text;

    switch(signal_val_type) {
    case svt_integer: text = "integer"; break;
    case svt_float:   text = "float";   break;
    case svt_double:  text = "double";  break;
    }

    return text;
}

QString show_attribute(attribute_t *a)
{
    QString result;
    if(a->name) {
        result += QString("%1=").arg(a->name);
    } else {
        result += QString("undef");
    }
    switch(a->value->value_type) {
    case vt_integer:
        result += QString("%1(INT)").arg(a->value->value.int_val);
        break;
    case vt_float:
        result += QString("%1(DOUBLE)").arg(a->value->value.double_val);
        break;
    case vt_string:
        result += QString("%1(STRING)").arg(a->value->value.string_val);
        break;
    case vt_enum:
        result += QString("%1(ENUM)").arg(a->value->value.enum_val);
        break;
    case vt_hex:
        result += QString("%1(HEX)").arg(a->value->value.hex_val);
        break;
    }

    return result;
}

QString ATdbc_file2db::ATdbc_stringlist_to_qstring(string_list_t *string_list)
{
    QString result = "";
    while (string_list) {
        result += QString(string_list->string);
        if (string_list->next != nullptr)
            result += ",";
        string_list = string_list->next;
    }
    return result;
}

QString ATdbc_file2db::ATdbc_attributelist_to_qstring(attribute_list_t *al)
{
    QString result;
    while (al)
    {
        if (al->attribute != nullptr)
            result += show_attribute(al->attribute);
        if (al->next != nullptr)
            result += ",";
        al = al->next;
    }
    return result;
}

QString ATdbc_file2db::Atdbc_valmap_to_qstring(val_map_t *vm)
{
    QString result="";
    while (vm)
    {
        result += QString("%1=\"%2\"").arg(vm->val_map_entry->index).arg(vm->val_map_entry->value);
        if(vm->next != nullptr)
            result += ",";
        vm = vm->next;
    }
    return result;
}

void ATdbc_file2db::show_valtable_list(valtable_list_t *valtable_list)
{
    QSqlQuery sql_exec;
    sql_exec.exec("drop table valtable");
    sql_exec.exec("create table valtable ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "valtable_name text, valtable_comment text, valtable_valmap text)");

    sql_exec.prepare("insert into valtable (valtable_name, valtable_comment, valtable_valmap) "
                     "values (?, ?, ?)");

    for(; valtable_list!=nullptr; valtable_list=valtable_list->next) {
        sql_exec.addBindValue(valtable_list->valtable->name);
        sql_exec.addBindValue(valtable_list->valtable->comment?valtable_list->valtable->comment:"");
        sql_exec.addBindValue(Atdbc_valmap_to_qstring(valtable_list->valtable->val_map));
        sql_exec.exec();
    }
}

void ATdbc_file2db::show_network(dbc_t *dbc)
{
    QSqlQuery sql_query;
    sql_query.exec("drop table network");
    if(!sql_query.exec("create table network(id integer primary key autoincrement, network text, network_comment text, network_attribute_list text)"))
    {
        qDebug() << "Error: Fail to create table."<< sql_query.lastError();
    }

    sql_query.prepare("insert into network (network, network_comment, network_attribute_list) "
                      "values (?, ?, ?)");

    sql_query.addBindValue(dbc->filename);
    if (dbc->network)
    {
        if (dbc->network->comment)
            sql_query.addBindValue(dbc->network->comment);
        else
            sql_query.addBindValue("");

        sql_query.addBindValue(ATdbc_attributelist_to_qstring(dbc->network->attribute_list));
    }

    sql_query.exec();

}

void ATdbc_file2db::show_signal(QSqlQuery &sqlexec, signal_list_t *sl)
{
    sqlexec.addBindValue(sl->signal->name);
    sqlexec.addBindValue(decode_mux_type(sl->signal->mux_type));
    sqlexec.addBindValue(sl->signal->mux_value);
    sqlexec.addBindValue(sl->signal->bit_start);
    sqlexec.addBindValue(sl->signal->bit_len);
    sqlexec.addBindValue(sl->signal->endianess);
    sqlexec.addBindValue(sl->signal->signedness);
    sqlexec.addBindValue(sl->signal->scale);
    sqlexec.addBindValue(sl->signal->offset);
    sqlexec.addBindValue(sl->signal->min);
    sqlexec.addBindValue(sl->signal->max);
    sqlexec.addBindValue(decode_signal_val_type(sl->signal->signal_val_type));
    sqlexec.addBindValue(sl->signal->unit?sl->signal->unit:"");
    sqlexec.addBindValue(ATdbc_stringlist_to_qstring(sl->signal->receiver_list));
    sqlexec.addBindValue(sl->signal->comment?sl->signal->comment:"");
    sqlexec.addBindValue(ATdbc_attributelist_to_qstring(sl->signal->attribute_list));
    sqlexec.addBindValue(Atdbc_valmap_to_qstring(sl->signal->val_map));
}

void ATdbc_file2db::show_message(QSqlQuery &sqlexec, message_list_t *ml)
{
//    sqlexec.bindValue(":message_id", ml->message->id);
//    sqlexec.bindValue(":message_name", ml->message->name);
//    sqlexec.bindValue(":message_len", ml->message->len);
//    sqlexec.bindValue(":message_sender", ml->message->sender);
//    sqlexec.bindValue(":message_comment", ml->message->comment);
//    sqlexec.bindValue(":message_attribute_list", ATdbc_attributelist_to_qstring(ml->message->attribute_list));
//    sqlexec.bindValue(":message_transmitter_list", ATdbc_stringlist_to_qstring(ml->message->transmitter_list));
    sqlexec.addBindValue(ml->message->id);
    sqlexec.addBindValue(ml->message->name);
    sqlexec.addBindValue(ml->message->len);
    sqlexec.addBindValue(ml->message->sender);
    sqlexec.addBindValue(ml->message->comment);
    sqlexec.addBindValue(ATdbc_attributelist_to_qstring(ml->message->attribute_list));
    sqlexec.addBindValue(ATdbc_stringlist_to_qstring(ml->message->transmitter_list));
}

void ATdbc_file2db::show_message_list(message_list_t *message_list)
{
    QSqlQuery sql_exec;
    sql_exec.exec("drop table message;");
    sql_exec.exec("create table message ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "message_id text,"
                  "message_name text,"
                  "message_len integer,"
                  "message_sender text,"
                  "message_comment text,"
                  "message_attribute_list text,"
                  "message_transmitter_list text)");

//    sql_exec.prepare("insert into message "
//                     "(message_id, message_name, message_len, message_sender, message_comment, message_attribute_list, message_transmitter_list)"
//                     "values (:message_id, :message_name, :message_len, :message_sender, :message_comment, :message_attribute_list,"
//                     " :message_transmitter_list)");
    sql_exec.prepare("insert into message "
                     "(message_id, message_name, message_len, message_sender, message_comment, message_attribute_list, message_transmitter_list)"
                     "values (?,?,?,?,?,?,?)");

    while(message_list != nullptr) {
        show_message(sql_exec, message_list);
        sql_exec.exec();
        message_list = message_list->next;
    }
}

void ATdbc_file2db::show_signals(dbc_t *dbc)
{
    message_list_t *ml;
    signal_list_t *sl;

    QSqlQuery sql_exec;
    sql_exec.exec("drop table signals;");
    sql_exec.exec("create table signals ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "message_id text, message_name text, message_len integer, message_sender text,"
                  "message_comment text, message_attribute_list text, message_transmitter_list text,"
                  "signal_name text, signal_mux_type text, signal_mux_value integer, signal_bit_start integer,"
                  "signal_bit_len integer, signal_endianess integer, signal_signedness integer, signal_scale real,"
                  "signal_offset real, signal_min real, signal_max real, signal_val_type text, signal_unit text, "
                  "signal_receiver_list text, signal_comment text, signal_attribute_list text, signal_val_map text)");

    sql_exec.prepare("insert into signals "
                     "(message_id, message_name, message_len, message_sender, message_comment, message_attribute_list, "
                     "message_transmitter_list, signal_name, signal_mux_type, signal_mux_value, signal_bit_start, signal_bit_len, "
                     "signal_endianess, signal_signedness, signal_scale, signal_offset, signal_min, signal_max, signal_val_type, "
                     "signal_unit, signal_receiver_list, signal_comment, signal_attribute_list, signal_val_map) "
                     "values (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");

    for(ml = dbc->message_list; ml != nullptr; ml = ml->next) {
        for(sl = ml->message->signal_list; sl != nullptr; sl = sl->next) {
            show_message(sql_exec, ml);
            show_signal(sql_exec, sl);
            sql_exec.exec();
        }
    }

}

void ATdbc_file2db::show_nodes(dbc_t *dbc)
{
    QSqlQuery sql_exec;
    node_list_t *nl;

    sql_exec.exec("drop table nodes;");
    sql_exec.exec("create table nodes ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "node_name text, node_comment text, node_attribute_list text)");

    sql_exec.prepare("insert into nodes"
                     "(node_name, node_comment, node_attribute_list)"
                     "values (?,?,?)");

    for(nl = dbc->node_list; nl != nullptr; nl = nl->next) {
        sql_exec.addBindValue(nl->node->name);
        printf("%s", nl->node->name);
        if(nl->node->comment) {
            sql_exec.addBindValue(nl->node->comment);
        }
        else {
            sql_exec.addBindValue("");
        }
        sql_exec.addBindValue(ATdbc_attributelist_to_qstring(nl->node->attribute_list));
    }
}

void ATdbc_file2db::show_envvars(envvar_list_t *envvar_list)
{
    envvar_list_t *el;
    const char *string_from_et[] = {
        "INTEGER",
        "FLOAT",
        "STRING",
        "DATA",
    };
    const char *string_from_at[] = {
        "UNRESTRICTED",
        "READONLY",
        "WRITEONLY",
        "READWRITE",
    };
    QSqlQuery sql_exec;
    sql_exec.exec("drop table envvars;");
    sql_exec.exec("create table envvars ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "envvar_name text, envvar_type text, envvar_access text, envvar_min integer, "
                  "envvar_max integer, envvar_unit text, envvar_initial integer, "
                  "envvar_index integer envvar_nodelist text, envvar_valmap text)");

    sql_exec.prepare("insert into envvars "
                     "(envvar_name, envvar_type, envvar_access, envvar_min,envvar_max, envvar_unit, "
                     "envvar_initial,envvar_index, envvar_nodelist, envvar_valmap)"
                     "values (?, ?, ?, ?, ?, ?, ?, ?, ?)");

    for(el = envvar_list; el != nullptr; el = el->next) {
        sql_exec.addBindValue(el->envvar->name);
        sql_exec.addBindValue(string_from_et[el->envvar->envtype]);
        sql_exec.addBindValue(string_from_at[el->envvar->access]);
        sql_exec.addBindValue(el->envvar->min);
        sql_exec.addBindValue(el->envvar->max);
        sql_exec.addBindValue(el->envvar->unit);
        sql_exec.addBindValue(el->envvar->initial);
        sql_exec.addBindValue(el->envvar->index);

        /* print node list */
        sql_exec.addBindValue(ATdbc_stringlist_to_qstring(el->envvar->node_list));
        /* print value map */
        sql_exec.addBindValue(Atdbc_valmap_to_qstring(el->envvar->val_map));
        sql_exec.exec();
    }
}

int ATdbc_file2db::ATdbc_filehandle(QString filename)
{
    dbc_t *dbc;
    dbc = dbc_read_file(const_cast<char *>(filename.toStdString().c_str()));
    if (nullptr != dbc)
    {
        show_network(dbc);
        show_message_list(dbc->message_list);
        show_signals(dbc);
        show_nodes(dbc);
        show_envvars(dbc->envvar_list);
        show_valtable_list(dbc->valtable_list);
        dbc_free(dbc);
    }
    else {
        qDebug()<<"test";
    }

    return 0;
}

int ATdbc_file2db::ATdbc_dbinit()
{
    db_handle = QSqlDatabase::addDatabase("QSQLITE");
    db_handle.setDatabaseName("./memory.db");
    if (!db_handle.open())
    {
         qDebug() << "Error: Failed to connect database." << db_handle.lastError();
         qDebug() << db_handle.drivers();
    }
    else
    {
        qDebug() << "Succeed to connect database." ;
    }
    return 0;
}

ATdbc_file2db::ATdbc_file2db(QString dbcfile)
{
    qDebug()<<dbcfile;

    ATdbc_dbinit();
    ATdbc_filehandle(dbcfile.toStdString().c_str());
}

ATdbc_file2db::~ATdbc_file2db()
{

}
