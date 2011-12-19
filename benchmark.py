""" thechess, chess game web application written in C++ and based on Wt
Copyright (C) 2010 Boris Nagaev

See the LICENSE file for terms of use.
"""

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

