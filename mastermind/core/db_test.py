from db_utils import create_table, drop_table, insert_listener_record
from exception_utils import printerr, TracebackContext


def main():
    listeners_schema = [
        "listener_id int AUTO_INCREMENT PRIMARY KEY NOT NULL ",
        "listener_name VARCHAR(25) NOT NULL",
        "listener_ip INT UNSIGNED NOT NULL",
        "listener_port SMALLINT UNSIGNED NOT NULL",
        "status ENUM('Started', 'Stopped') NOT NULL",
        "FOREIGN KEY (listener_id) REFERENCES Agents (agent_id) ",
    ]
    agents_schema = [
        "agent_id int AUTO_INCREMENT PRIMARY KEY NOT NULL ",
        "agent_uuid BINARY(16) NOT NULL",
        "listener_id int NOT NULL",
        "tgt_ip INT UNSIGNEDNOT NULL",
        "tgt_hostname VARCHAR(255) NOT NULL",
        "tgt_os VARCHAR(15) NOT NULL",
        "tgt_version VARCHAR(20) NOT NULL",
        "CONSTRAINT uniq_uuid UNIQUE(agent_uuid)",
        "INDEX (agent_uuid)",
        "FOREIGN KEY (agent_id) REFERENCES Listeners (listener_id) ",
        # "FOREIGN KEY (agent_uuid) REFERENCES Tasks (agent_uuid) ",
    ]

    tasks_schema = [
        "task_id INT UNSIGNED PRIMARY KEY NOT NULL",
        "agent_uuid BINARY(16) NOT NULL",
        "task_type ENUM('cmd', 'sleep', 'rename', 'shell') NOT NULL",
        "task_arguments VARCHAR(50)",
        "task_status ENUM('issued', 'collected', 'complete') NOT NULL",
        "task_results TEXT",
        "CONSTRAINT uniq_uuid UNIQUE(agent_uuid)",
        "INDEX (agent_uuid)",
        "FOREIGN KEY (agent_uuid) REFERENCES Agents (agent_uuid) ",
    ]

# https://stackoverflow.com/questions/69921907/failed-to-add-the-foreign-key-constraint-missing-index-for-constraint-informat

    # drop_table("Listeners")
    # drop_table("Agents")
    # drop_table("Tasks")

    # create_table("Listeners", listeners_schema, 1)
    # create_table("Agents", agents_schema, 1)
    # create_table("Tasks", tasks_schema)

    # insert_listener_record()
    insert_listener_record("listener 17", "192.200.2.90", 1000, 'Stopped')


if __name__ == "__main__":
    with TracebackContext(True):
        main()
