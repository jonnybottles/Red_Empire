from mysql_utils import get_connection
from exception_utils import printerr, TracebackContext
from mysql.connector.errors import ProgrammingError

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


def main():
    listeners_columns = [
        "listener_id int AUTO_INCREMENT PRIMARY KEY NOT NULL ",
        "listener_name VARCHAR(25) NOT NULL",
        "listener_ip INT UNSIGNED NOT NULL",
        "listener_port SMALLINT UNSIGNED NOT NULL",
        "status ENUM('Started', 'Stopped') NOT NULL",
        "FOREIGN KEY (listener_id) REFERENCES Agents (agent_id) ",
    ]
    agents_columns = [
        "agent_id int AUTO_INCREMENT PRIMARY KEY NOT NULL ",
        "agent_uuid BINARY(16) NOT NULL",
        "listener_id int NOT NULL",
        "tgt_ip INT UNSIGNED NOT NULL",
        "tgt_hostname VARCHAR(255) NOT NULL",
        "tgt_os VARCHAR(15) NOT NULL",
        "tgt_version VARCHAR(20) NOT NULL",
        "FOREIGN KEY (agent_id) REFERENCES Listeners (listener_id) ",
        "FOREIGN KEY (agent_uuid) REFERENCES Tasks (agent_uuid) ",
    ]

    tasks_columns = [
        "task_id INT UNSIGNED PRIMARY KEY NOT NULL",
        "agent_uuid BINARY(16) NOT NULL",
        "task_type ENUM('cmd', 'sleep', 'rename', 'shell') NOT NULL",
        "task_arguments VARCHAR(50)",
        "task_status ENUM('issued', 'collected', 'complete') NOT NULL",
        "task_results TEXT",
        "FOREIGN KEY (agent_uuid) REFERENCES Agents (agent_uuid) ",
    ]

    create_table("Listeners", listeners_columns, 1)
    create_table("Agents", agents_columns, 1)
    create_table("Tasks", tasks_columns)


if __name__ == "__main__":
    with TracebackContext(False):
        main()
