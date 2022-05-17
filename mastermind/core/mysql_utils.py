import mysql.connector


def get_connection(verbose=False):
    login_name = "redalert"
    config = {"user": login_name, "database": login_name, "password": login_name}
    connection = mysql.connector.connect(**config)
    if verbose:
        print("Connecting with configuration:", config, sep="\n")
        print("Using", type(connection))
    return connection


if __name__ == "__main__":
    connection = get_connection(True)
    connection.close()
