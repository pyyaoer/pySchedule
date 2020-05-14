import json
import datetime
import subprocess
import time

data = {
"PNODE_NUM" : 1,
"GATE_NUM" : 8,
"USER_NUM" : 12,
"TENANT_NUM" : 4,
"THREADS_PER_GATE" : 2,
"TENANT_LIMIT" : 150,
"TENANT_RESERVATION" : 90,
"BENCHMARK_SHAPE": 0,
"MSG_LATENCY": 10,
"GATE_PERIOD": 10000,
"WINDOW_SIZE": 10,
"REQUEST_HARDNESS": 100 
}

cmds=["../bin/base", "../bin/batch"]

for i in range(0, 2):
    print("i="+str(i))
    for j in range(0, 12):
        data["MSG_LATENCY"] = j*50
        res = json.dumps(data)
        
        f = open('config.json', "w")
        f.write(res)
        f.close()
        
        f = open('tmp.txt', "w")
        sub = subprocess.Popen([cmds[i], "config.json"], stdout = f.fileno())
        sub.poll()
        time.sleep(10)
        sub.kill()
        f.flush()
        f.close()
        with open('tmp.txt', 'r') as fp:
            lines = fp.readlines()
            last_line = lines[-1]
            print(last_line.strip('\n'))
        time.sleep(1)
