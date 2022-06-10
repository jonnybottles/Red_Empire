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
    with open(agent.tasks_path, 'r') as file:
        lines = file.readlines()
    file.close()

    with open(agent.tasks_path, 'w') as file:
        for line in lines:
            # find() returns -1 if no match is found
            if line.find(task_id) != -1:
                pass
            else:
                file.write(line)
        file.close()
    # If file has only 2 lines, no tasks are present, only $ EOF marker / newline.
    with open(agent.tasks_path, 'r') as file:
        if(len(file.readlines()) == 1):
            agent.has_tasks = False
        file.close()

def update_log(agent, task_id, task_cmd):
    # previousw = f" {task_id}         issued                 cmd                   {task_cmd}"
    # nextwwwww = f" {task_id}         complete               cmd                   {task_cmd}\n"

    # text = f"{task_id}"
    # new_text = f" {task_id}         complete               cmd                   {task_cmd}"

    file_path = agent.tasks_log_path

    #Create temp file
    fd, abs_path = mkstemp()
    with fdopen(fd, 'w') as new_file:
        with open(file_path, 'r') as old_file:
            for line in old_file:
                if task_id in line:
                    new_file.write(line.replace("assigned", "complete"))
                else:
                    new_file.write(line)
                
    #Copy the file permissions from the old file to the new file
    copymode(file_path, abs_path)
    
    #Remove original file
    remove(file_path)
    
    #Move new file
    move(abs_path, file_path)



    # file = open(agent.tasks_log_path, "r")
    # replaced_content = ""
    # #looping through the file
    # for line in file:
        
    #     #stripping line break
    #     line = line.strip()
    #     #replacing the texts
    #     if task_id in line:
    #         line = line.replace("issued", "complete")
    #     #concatenate the new string and add an end-line break
    #         replaced_content = replaced_content + line + "\n"
        
    # #close the file
    # file.close()
    # #Open file in write mode
    # write_file = open(agent.tasks_log_path, "w")
    # #overwriting the old file contents with the new/replaced content
    # write_file.write(replaced_content)
    # #close the file
    # write_file.close()



#    file_path = agent.tasks_log_path
#    #Create temp file
#    fh, abs_path = mkstemp()
#    new_file = open(abs_path,'w')
#    old_file = open(file_path)
#    for line in old_file:
#        new_file.write(line.replace(pattern, subst))
#    #close temp file
#    new_file.close()
#    close(fh)
#    old_file.close()
#    #Remove original file
#    remove(file_path)
#    #Move new file
#    move(abs_path, file_path)

    # x = fileinput.input(files=agent.tasks_log_path, inplace=1)
    # for line in x:
    #     if text in line:
    #         line = new_text
    # x.close()

    # for line in fileinput.input(agent.tasks_log_path, inplace=1):
    #    line = line.replace(previousw, nextwwwww)
    #    sys.stdout.write(line)



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
        # print(type(agents[agent.name]))
        # print(f"agents listener name{agents[agent.name].listener_name}")
        # print(f"Agent dict len is in register agent is {len(agents)}")
    except:
        pass

def check_agents_empty(s):
    
    # uagents()

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

    # uagents()
    valid_agents = []
    print(f"Agent dict len is in check agent empty is {len(agents)}")
    for agent in agents:
        valid_agents.append(agents[agent].name)
        print(f" Agent name in is_valid_agent: {agents[agent].name}")

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
        print(" UUID                                        Listener                          Target IP                          Hostname")
        print("------                                      ----------                       -------------                       ----------")
        
        for i in agents:
            print(f" {agents[i].name}" + " " * (44 - len(str(agents[i].name))) + f"{agents[i].listener_name}" + " " * (33 - len(agents[i].listener_name)) + agents[i].tgt_ip + " " * (36 - len(agents[i].tgt_ip)) + agents[i].tgt_hostname)
        
        print(cRESET)

def remove_agent(args):
    
    if len(args) != 1:
        error("Invalid arguments.")
    else:
        name = args[0]
        if is_valid_agent(name, 1):
            # task_agent_to_quit(name)
            rmtree(agents[name].path)
            # removeFromDatabase(agentsDB,name)
            # uagents()
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
        # print("$$Agent UUID from dict is:\n", key)
        return key