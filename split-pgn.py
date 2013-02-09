# splits multiple pgn file to files and dirs (1000 games per dir)

import re
import os

n = None
f = None
while True:
    line = raw_input()
    if re.search('^\[Event "(\d+)"\]', line):
        n = int(re.search('^\[Event "(\d+)"\]', line).groups()[0])
        if f:
            f.close()
        d = str(n / 1000)
        if not os.path.exists(d):
            os.mkdir(d)
        name = d + '/' + str(n) + '.pgn'
        f = open(name, 'w')
    if f:
        f.write(line+"\n")

