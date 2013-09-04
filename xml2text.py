#!/usr/bin/python

import sys
from xml.etree.ElementTree import parse
try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

contents = sys.stdin.read().replace('&', 'ampersand').replace('if:', '')
xml_file = StringIO(contents)
root = parse(xml_file).getroot()

def explore(element):
    text = element.text
    if text and text.strip():
        print(text.strip().encode('utf8'))
    tail = element.tail
    if tail and tail.strip():
        print(tail.strip().encode('utf8'))
    for child in element:
        explore(child)

explore(root)

