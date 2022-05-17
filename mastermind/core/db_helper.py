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


def create_table(table_name, column_list, auto_increment=None):
    connection = None
    try:
        connection = get_connection(verbose=True)
        cursor = connection.cursor()

        fkey_chk = "SET foreign_key_checks = 0;"
        cursor.execute(fkey_chk)

        dlt = f"DROP TABLE IF EXISTS {table_name}"
        cursor.execute(dlt)

        sql = f"CREATE TABLE {table_name} (  {column_list[0]})"
        cursor.execute(sql)
        if len(column_list) > 1:
            for column in column_list[1:]:
                sql = f"ALTER TABLE {table_name} ADD {column}"
                cursor.execute(sql)
        if not auto_increment:
            sql = f"ALTER TABLE {table_name} AUTO_INCREMENT={auto_increment}"
    except ProgrammingError as pe:
        printerr("Unable to establish connection to database:\n", pe)
    finally:
        if connection:
            cursor.close()
            connection.close()