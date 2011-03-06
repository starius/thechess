
import urllib2
import re
import sys

n = int(sys.argv[1])
url = sys.argv[2]

sessions = []

for i in range(n):
    text = urllib2.urlopen(url).read().replace('&amp;', '&')
    try:
        url1 = re.search(r'\?wtd=\w+\&js=no', text).group()
        print 'start', url1
        text = urllib2.urlopen(url + url1).read()
        sessions.append(text)
    except:
        pass

while True:
    for i in range(len(sessions)):
        text = sessions[i]
        url1 = re.search(r'\?wtd=\w+', text).group()
        print url1
        text = urllib2.urlopen(url + url1).read()
        sessions[i] = text


