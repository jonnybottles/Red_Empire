from db_utils import create_table, drop_table, insert_listener_record, insert_agent_record
from exception_utils import printerr, TracebackContext


def main():
    listeners_schema = [
        # "listener_name int AUTO_INCREMENT PRIMARY KEY NOT NULL ",
        "listener_name VARCHAR(25) PRIMARY KEY NOT NULL",
        "listener_ip INT UNSIGNED NOT NULL",
        "listener_port SMALLINT UNSIGNED NOT NULL",
        "listener_interface VARCHAR(10) NOT NULL",
        "status ENUM('Started', 'Stopped') NOT NULL",
        "CONSTRAINT uniq_listener UNIQUE(listener_name)",
        # "FOREIGN KEY (listener_id) REFERENCES Agents (agent_uuid) ",
    ]
    agents_schema = [
        # "agent_id int AUTO_INCREMENT PRIMARY KEY NOT NULL ",
        "agent_uuid VARCHAR(36) PRIMARY KEY NOT NULL",
        "listener_name VARCHAR(25) NOT NULL",
        "tgt_ip INT UNSIGNED NOT NULL",
        "tgt_hostname VARCHAR(255) NOT NULL",
        "tgt_os VARCHAR(15) NOT NULL",
        "tgt_version VARCHAR(20) NOT NULL",
        "CONSTRAINT uniq_uuid UNIQUE(agent_uuid)",
        "INDEX (agent_uuid)",
        "FOREIGN KEY (listener_name) REFERENCES Listeners (listener_name) ",
        # "FOREIGN KEY (agent_uuid) REFERENCES Tasks (agent_uuid) ",
    ]

    tasks_schema = [
        "task_id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY NOT NULL",
        "agent_uuid VARCHAR(36) NOT NULL",
        "task_type ENUM('cmd', 'sleep', 'rename', 'shell') NOT NULL",
        "task_arguments VARCHAR(50)",
        "task_status ENUM('issued', 'collected', 'complete') NOT NULL",
        "task_results TEXT",
        "INDEX (agent_uuid)",
        "FOREIGN KEY (agent_uuid) REFERENCES Agents (agent_uuid) ",
    ]

# https://stackoverflow.com/questions/69921907/failed-to-add-the-foreign-key-constraint-missing-index-for-constraint-informat

    # drop_table("Listeners")
    # drop_table("Agents")
    # drop_table("Tasks")

    # create_table("Listeners", listeners_schema)
    # create_table("Agents", agents_schema)
    # create_table("Tasks", tasks_schema, 1)

    insert_listener_record("listener_34", "192.200.2.90", 1000, "eth0", "Started")
    insert_listener_record("listener_17", "192.200.2.91", 9999, "eth1", "Stopped")
    insert_agent_record("listener_34", "120.17.29.2", "EXCH02", "Ubuntu", "20.04")
    insert_agent_record("listener_34", "8.8.8.8", "DC01", "Windows", "SVR2016")
    insert_agent_record("listener_17", "174.17.8.2", "ADMIN_457", "Windows", "Windows10")

if __name__ == "__main__":
    with TracebackContext(True):
        main()
