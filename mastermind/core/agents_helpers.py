from .common import *
from shutil import rmtree
import fileinput
import sys
from tempfile import mkstemp
from shutil import move, copymode
from os import fdopen, remove

# NEED TO ADD LOCKS HERE WHEN I START WORKING WITH MULTIPLE THREADS
agents = {}


def delete_task(agent, task_id):
    with open(agent.tasks_path, "r") as file:
        lines = file.readlines()
    file.close()

    with open(agent.tasks_path, "w") as file:
        for line in lines:
            # find() returns -1 if no match is found
            if line.find(task_id) != -1:
                pass
            else:
                file.write(line)
        file.close()
    # If file has only 2 lines, no tasks are present, only $ EOF marker / newline.
    with open(agent.tasks_path, "r") as file:
        if len(file.readlines()) == 1:
            agent.has_tasks = False
        file.close()


def update_log(agent, task_id, task_cmd):
    file_path = agent.tasks_log_path

    # Create temp file
    fd, abs_path = mkstemp()
    with fdopen(fd, "w") as new_file:
        with open(file_path, "r") as old_file:
            for line in old_file:
                if task_id in line:
                    new_file.write(line.replace("assigned", "complete"))
                else:
                    new_file.write(line)

    # Copy the file permissions from the old file to the new file
    copymode(file_path, abs_path)

    # Remove original file
    remove(file_path)

    # Move new file
    move(abs_path, file_path)


def write_results(task_id, task_cmd, task_results, agent):
    agent = get_agent_object(agent)
    with open(f"{agent.results_path}/{task_id}.txt", "w") as f:
        f.write(f"{task_results}")
    delete_task(agent, task_id)
    update_log(agent, task_id, task_cmd)


def add_agent(agent):
    # Need to do more error checking here.
    global agents
    try:

        agents[agent.name] = agent
    except:
        pass


def check_agents_empty(s):
    print(f"Agent dict len is in check agent empty is {len(agents)}")

    if len(agents) == 0:

        if s == 1:
            error("There are no active agents.")
            return True
        else:
            return True

    else:
        return False


def is_valid_agent(name, s):
    valid_agents = []
    for agent in agents:
        valid_agents.append(agents[agent].name)

    if name in valid_agents:
        return True
    else:
        if s == 1:
            error("Invalid agent.")
            return False
        else:
            return False


def view_agents():

    if check_agents_empty(1) == False:

        success("Active Agents:")

        print(YELLOW)
        print(
            " UUID                                        Listener                          Target IP                          Hostname"
        )
        print(
            "------                                      ----------                       -------------                       ----------"
        )

        for i in agents:
            print(
                f" {agents[i].name}"
                + " " * (44 - len(str(agents[i].name)))
                + f"{agents[i].listener_name}"
                + " " * (33 - len(agents[i].listener_name))
                + agents[i].tgt_ip
                + " " * (36 - len(agents[i].tgt_ip))
                + agents[i].tgt_hostname
            )

        print(cRESET)


def remove_agent(args):

    if len(args) != 1:
        error("Invalid arguments.")
    else:
        name = args[0]
        if is_valid_agent(name, 1):
            rmtree(agents[name].path)
        else:
            pass


def get_agent_object(name):
    return agents.get(name)


def get_agents_for_listener(name):

    result = []

    for agent in agents:
        if agents[agent].listener.name == name:
            result.append(agents[agent].name)

    return result


def task_agent(args):

    if len(args) != 1:
        error("Invalid arguments.")
    else:
        name = args[0]
        if is_valid_agent(name, 1):
            agents[name].task_agent()
        else:
            pass


def get_agent_uuid():
    for key, value in agents.items():
        return key
