import json
import datetime
import subprocess
import time
import copy

data_raw = {
"PNODE_NUM" : 1,
"GATE_NUM" : 8,
"USER_NUM" : 12,
"TENANT_NUM" : 4,
"THREADS_PER_GATE" : 2,
"TENANT_LIMIT" : 150,
"TENANT_RESERVATION" : 90,
"BENCHMARK_SHAPE": 1,
"MSG_LATENCY": 10,
"GATE_PERIOD": 1000,
"WINDOW_SIZE": 10,
"REQUEST_HARDNESS": 100,
"DBG":0
}

cmds=["../bin/base", "../bin/batch", "../bin/autoadjust"]

def run(i, data):
    res = json.dumps(data)
    
    f = open('config.json', "w")
    f.write(res)
    f.close()
    
    f = open('tmp.txt', "w")
    sub = subprocess.Popen([cmds[i], "config.json"], stdout = f.fileno())
    sub.poll()
    time.sleep(120)
    sub.kill()
    f.flush()
    f.close()
    time.sleep(1)
    with open('tmp.txt', 'r') as fp:
        lines = fp.readlines()
        last_line = lines[-1]
        return last_line.strip('\n').split()

def fun(shape, s, e, sz, change):
    data = copy.deepcopy(data_raw)
    data["BENCHMARK_SHAPE"] = shape
    index = []
    latency = []
    accuracy = []
    for i in range(0, sz):
        idx, data = change(i, data)
        index.append(idx)
        for j in range(0, 10):
            latency.append([])
            accuracy.append([])
        for j in range(s, e):
            res = run(j, data)
            #print(str(res[0]) + " " + str(res[1]))
            latency[j].append(res[0])
            accuracy[j].append(res[1])
    for i in range(len(index)):
        print index[i],
        for j in range(s, e):
            print latency[j][i],
        print "\n",
    print "\n"
    for i in range(len(index)):
        print index[i],
        for j in range(s, e):
            print accuracy[j][i],
        print "\n",

def latency_change(i, data):
    idx = 50*i
    data["MSG_LATENCY"] = idx
    return idx, data

def period_change(i, data):
    idx = 1000*(i+1)
    data["GATE_PERIOD"]=idx
    return idx, data

def period_change_100(i, data):
    idx = 100*(i+1)
    data["GATE_PERIOD"]=idx
    return idx, data

#fun(0, 1, 3, 10, period_change)
#fun(0, 0, 2, 11, latency_change)

print "+++++++++ period ++++++++++++"
for i in range(0, 4):
    print "shape = " + str(i)
    print "\t********period_100********"
    fun(i, 2, 3, 11, period_change_100)
    print "\t********period_1000********"
    fun(i, 2, 3, 11, period_change)

#print "+++++++++ latency ++++++++++++"
#for i in range(0, 4):
#    print "shape = " + str(i)
#    fun(i, 0, 2, 11, latency_change)

