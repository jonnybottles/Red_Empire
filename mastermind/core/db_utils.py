from socket import inet_aton
import mysql.connector
from exception_utils import printerr, TracebackContext
from mysql.connector.errors import ProgrammingError


def get_connection(verbose=False):
    login_name = "redalert"
    config = {"user": login_name, "database": login_name, "password": login_name}
    connection = mysql.connector.connect(**config)
    if verbose:
        print("Connecting with configuration:", config, sep="\n")
        print("Using", type(connection))
    return connection


def create_table(table_name, schema, auto_increment=None):
    connection = None
    try:
        connection = get_connection(verbose=True)
        cursor = connection.cursor()

        drop_table(table_name)

        fkey_chk = "SET foreign_key_checks = 0;"
        cursor.execute(fkey_chk)

        sql = f"CREATE TABLE {table_name} (  {schema[0]})"
        cursor.execute(sql)
        if len(schema) > 1:
            for column in schema[1:]:
                alter_table(table_name, column)

        if auto_increment:
            sql = f"ALTER TABLE {table_name} AUTO_INCREMENT={auto_increment}"
    except ProgrammingError as pe:
        printerr("Unable to establish connection to database:\n", pe)
    finally:
        if connection:
            cursor.close()
            connection.close()

def alter_table(table_name, alteration):
    connection = None
    try:
        connection = get_connection()
        cursor = connection.cursor()

        fkey_chk = "SET foreign_key_checks = 0;"
        cursor.execute(fkey_chk)

        alt = f"ALTER TABLE {table_name} ADD {alteration}"
        cursor.execute(alt)

    except ProgrammingError as pe:
        printerr("Unable to establish connection to database:\n", pe)
    finally:
        if connection:
            cursor.close()
            connection.close()


def drop_table(table_name):
    connection = None
    try:
        connection = get_connection()
        cursor = connection.cursor()
        
        fkey_chk = "SET foreign_key_checks = 0;"
        cursor.execute(fkey_chk)

        dlt = f"DROP TABLE IF EXISTS {table_name}"
        cursor.execute(dlt)

    except ProgrammingError as pe:
        printerr("Unable to establish connection to database:\n", pe)
    finally:
        if connection:
            cursor.close()
            connection.close()

def insert_listener_record(listener_name, listener_ip, listener_port, listener_interface, listener_status):
    try:
        connection = get_connection()
        sql = "INSERT INTO Listeners (listener_name, listener_ip, listener_port, listener_interface, status) VALUES (%s, INET_ATON(%s), %s, %s, %s)"

        cursor = connection.cursor()
        
        fkey_chk = "SET foreign_key_checks = 0;"
        cursor.execute(fkey_chk)
        cursor.execute(sql, (listener_name, listener_ip, listener_port, listener_interface, listener_status))
        connection.commit()
    except ProgrammingError as pe:
        printerr("Unable to establish connection to database:\n", pe)
        connection.rollback()
    finally:
        if connection:
            cursor.close()
            connection.close()

def insert_agent_record(listener_name, tgt_ip, tgt_hostname, tgt_os, tgt_version):
    try:
        connection = get_connection()
        sql = "INSERT INTO Agents (agent_uuid, listener_name, tgt_ip, tgt_hostname, tgt_os, tgt_version) VALUES (UUID(), %s, INET_ATON(%s), %s, %s, %s)"

        cursor = connection.cursor()
        
        fkey_chk = "SET foreign_key_checks = 0;"
        cursor.execute(fkey_chk)
        cursor.execute(sql, (listener_name, tgt_ip, tgt_hostname, tgt_os, tgt_version))
        connection.commit()
    except ProgrammingError as pe:
        printerr("Unable to establish connection to database:\n", pe)
        connection.rollback()
    finally:
        if connection:
            cursor.close()
            connection.close()

# def insert_task_record(listener_name, tgt_ip, tgt_hostname, tgt_os, tgt_version):
#     try:
#         connection = get_connection()
#         sql = "INSERT INTO Agents (agent_uuid, listener_name, tgt_ip, tgt_hostname, tgt_os, tgt_version) VALUES (UUID(), %s, INET_ATON(%s), %s, %s, %s)"

#         cursor = connection.cursor()
        
#         fkey_chk = "SET foreign_key_checks = 0;"
#         cursor.execute(fkey_chk)
#         cursor.execute(sql, (listener_name, tgt_ip, tgt_hostname, tgt_os, tgt_version))
#         connection.commit()
#     except ProgrammingError as pe:
#         printerr("Unable to establish connection to database:\n", pe)
#         connection.rollback()
#     finally:
#         if connection:
#             cursor.close()
#             connection.close()


def delete_data(table_name):
    connection = None
    try:
        connection = get_connection()
        cursor = connection.cursor()
        
        fkey_chk = "SET foreign_key_checks = 0;"
        cursor.execute(fkey_chk)

        dlt = f"DELETE FROM {table_name};"
        cursor.execute(dlt)
        connection.commit()

    except ProgrammingError as pe:
        printerr("Unable to establish connection to database:\n", pe)
        connection.rollback()
    finally:
        if connection:
            cursor.close()
            connection.close()


# def insert_record():
#     connection = get_connection()
#     sql = "INSERT INTO people VALUES (%s, %s, %s)"
#     cursor = connection.cursor()
#     cursor.execute(sql, ("Bob", "developer", 50000))
#     connection.commit()
#     cursor.close()
#     connection.close()

# def rowin(self, table_name, ColumnData=[]):
#     # First check number columns in the table TableName to confirm ColumnData=[] fits
#     check = f"SELECT count(*) FROM information_schema.columns WHERE table_name = {table_name}"
#     connection = get_connection()
#     cursor = connection.cursor()
#     cursor.execute(check)
#     col_count = len(cursor.fetchall())
#     # Compare TableName Column count to len(ColumnData)

#     if col_count == len(ColumnData):
#         # I want to be have the number of ? = ColCount
#         connection.executemany('''INSERT INTO {tn} VALUES (%s, %s)'''.format(tn=table_name), ColumnData)
#         connection.commit()

#     else:
#         print("Input doesn't match number of columns")