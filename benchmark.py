
import urllib2
import re
import sys

url = sys.argv[1]

while True:
    text = urllib2.urlopen(url).read().replace('&amp;', '&')
    try:
        print text
        url1 = re.search(r'\?wtd=\w+\&js=no', text).group()
        text = urllib2.urlopen(url + url1).read()
        print text
    except:
        pass

