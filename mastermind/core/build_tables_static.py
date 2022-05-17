from mysql_utils import get_connection
from exception_utils import printerr, TracebackContext
from mysql.connector.errors import ProgrammingError

# def create_table(table_name, table_column, datatypes):
#     connection = get_connection()
#     sql = (f"CREATE TABLE people (""  name VARCHAR(64), job VARCHAR(32), pay INTEGER"")")
#     cursor = connection.cursor()
#     cursor.execute(sql)
#     cursor.close()
#     connection.close()

def create_table():
    connection = None
    try:
        connection = get_connection(verbose=True)
        cursor = connection.cursor()
        dlt = "DROP TABLE IF EXISTS listener"
        sql = ("CREATE TABLE listener ("
               "  listener_name VARCHAR(25) NOT NULL, listener_ip INT UNSIGNED NOT NULL, listener_port SMALLINT UNSIGNED NOT NULL"
               ")")
        cursor.execute(dlt)
        cursor.execute(sql)
        cursor.close()
    except ProgrammingError as pe:
        printerr("Unable to establish connection to database:\n", pe)
    finally:
        if connection:
            connection.close()


def main():
    create_table()

if __name__ == "__main__":
    with TracebackContext(False):
        main()