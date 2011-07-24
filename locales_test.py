
import codecs
import os.path
from xml.etree.ElementTree import parse
from StringIO import StringIO
import fnmatch
import os
import re

try:
    wt_messages = parse(open("wt.xml")).getroot()
except:
    print 'Error: put wt.xml file into current directory'
    quit()

wt_ids = set()
for message in wt_messages:
    wt_ids.add(message.get('id'))

sections = ['common', 'competition', 'game', 'time', 'user']

Ids = set()

for filename in os.listdir('locales'):
    path = os.path.join('locales', filename)
    xml_file = codecs.open(path, 'r', 'utf-8')
    if xml_file.read().count('\t'):
        print filename, 'Error: no tabs are allowed'
    xml_file.seek(0)
    for line in xml_file:
        if len(line) > 120:
            print filename, 'Error: line "%s" is too long' % line
    xml_file = StringIO(open(path).read().replace('&',''))
    messages = parse(xml_file).getroot()
    keys = set()
    for message in messages:
        Id = message.get('id')
        Ids.add(Id)
        text = message.text
        if message.tag != 'message':
            print filename, Id, 'Error: wrong tag "%s"' % message.tag
        if text and '\n' not in text and (text[0] == ' ' or text[-1] == ' '):
            print filename, Id, 'Error: message should not start or end with space'
        if Id in keys:
            print filename, Id, 'Error: duplicate message'
        keys.add(Id)
        if not Id.startswith('Wt.') and not Id.startswith('tc.'):
            print filename, Id, 'Error: message id should start with Wt. or tc.'
        if Id.startswith('Wt.'):
            if Id not in wt_ids:
                print filename, Id, 'Error: unknown Wt message (old wt.xml?)'
        else:
            tc, section, id = Id.split('.')
            if section not in sections:
                print filename, Id, 'Error: unknown section "%s"' % section
            if '-' in id:
                print filename, Id, 'Error: tc id should not contain "-"'
            if text and id[0].islower() != text[0].islower() and text[0].isalpha():
                print filename, Id, 'Error: tc id should start with letter of same case as message'
            if '${' in text and not id.endswith('_template'):
                print filename, Id, 'Error: id of tc template message should end with "_template"'
    short_messages = [m.get('id').lower() for m in messages
        if m.text and '\n' not in m.text and m.get('id').startswith('tc.')]
    for m1, m2 in zip(short_messages, short_messages):
        if m1 != m2:
            print filename, 'Error: messages are unsorted, started from %s != %s' % (m1, m2)

l9n_re = re.compile(r'"(tc\.[^"]+)"')

for root, dirnames, filenames in os.walk('src'):
    for filename in fnmatch.filter(filenames, '*.?pp'):
        path = os.path.join(root, filename)
        for match in re.findall(l9n_re, open(path).read()):
            if match not in Ids:
                print path, "Error: can't find message for id '%s'" % match



