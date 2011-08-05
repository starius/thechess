
import codecs
import os.path
from xml.etree.ElementTree import parse
from StringIO import StringIO
import fnmatch
import os
import re
import argparse

ampersand = '-thechess_ampersand'

p = argparse.ArgumentParser(description='Wt l9n checker')
p.add_argument('--wt', help='path to wt.xml', metavar='FILE', type=argparse.FileType('r'), required=True)
p.add_argument('--prefix', help='Current project prefix', metavar='STR', default='tc.')
p.add_argument('--sections', help='The list of allowed sections', nargs='+',
    default=['common', 'competition', 'game', 'time', 'user'])
args = p.parse_args()

try:
    wt_messages = parse(args.wt).getroot()
except:
    print 'Error: put wt.xml file into current directory'
    quit()

wt_ids = set()
for message in wt_messages:
    wt_ids.add(message.get('id'))

sections = args.sections

filename2ids = {}
id2text = {}

for filename in os.listdir('locales'):
    ids = filename2ids[filename] = set()
    path = os.path.join('locales', filename)
    xml_file = codecs.open(path, 'r', 'utf-8')
    if xml_file.read().count('\t'):
        print filename, 'Error: no tabs are allowed'
    xml_file.seek(0)
    for line in xml_file:
        if len(line) > 120:
            print filename, 'Error: line "%s" is too long' % line
    xml_file = StringIO(open(path).read().replace('&', ampersand))
    messages = parse(xml_file).getroot()
    prev_message = None
    for message in messages:
        Id = message.get('id')
        text = message.text
        if message.tag != 'message':
            print filename, Id, 'Error: wrong tag "%s"' % message.tag
        if text and '\n' not in text and (text[0] == ' ' or text[-1] == ' '):
            print filename, Id, 'Error: message should not start or end with space'
        if Id in ids:
            print filename, Id, 'Error: duplicate message'
        ids.add(Id)
        id2text[Id] = text
        if not Id.startswith('Wt.') and not Id.startswith(args.prefix):
            print filename, Id, 'Error: message id should start with Wt. or %s' % args.prefix
        if Id.startswith('Wt.'):
            if Id not in wt_ids:
                print filename, Id, 'Error: unknown Wt message (old wt.xml?)'
        else:
            try:
                prefix, section, id = Id.split('.')
            except:
                print filename, Id, "Error: can't match to %ssection.id" % args.prefix
            message.section = section
            if section not in sections:
                print filename, Id, 'Error: unknown section "%s"' % section
            if '-' in id:
                print filename, Id, 'Error: id should not contain "-"'
            if text and id[0].islower() != text[0].islower() and text[0].isalpha():
                print filename, Id, 'Error: id should start with letter of same case as message'
            if '${' in text and not id.endswith('_template'):
                print filename, Id, 'Error: id of template message should end with "_template"'
            if hasattr(prev_message, 'section'):
                if prev_message.section != section and '\n\n' not in prev_message.tail:
                    print filename, 'Error: sections %s and %s should be separated by empty line' % \
                        (prev_message.section, section)
                elif prev_message.section == section:
                    if '\n\n' in prev_message.tail:
                        print filename, 'Error: section %s should not be separated with empty line near %s' % \
                            (section, Id)
                    if '\n' in prev_message.text and '\n' not in message.text:
                        print filename, 'Error: non-multiline message %s follows multimessages' % Id
                    if '\n' in prev_message.text and '\n' in message.text \
                        and prev_message.get('id') > Id and not 'board' in Id:
                        print filename, 'Error: multiline messages %s and %s are unordered' % \
                            (prev_message.get('id'), Id)

        prev_message = message

    short_messages = [m.get('id').lower() for m in messages
        if m.text and '\n' not in m.text and m.get('id').startswith(args.prefix)]
    for m1, m2 in zip(short_messages, short_messages):
        if m1 != m2:
            print filename, 'Error: messages are unsorted, started from %s != %s' % (m1, m2)

all_ids = reduce(lambda a,b:a|b, filename2ids.values(), set())
cleaner = re.compile(r'{.+}|<.+>|%s.{1,8};' % ampersand)

for filename, ids in filename2ids.items():
    for Id in all_ids - ids:
        if filename == 'thechess.xml' and Id.startswith("Wt."):
            continue
        if not any(c.isalpha() for c in re.sub(cleaner, "", id2text[Id])):
            continue
        print filename, 'Warning: id "%s" not found' % Id

l9n_re = re.compile(r'"(%s[^"]+)"' % re.escape(args.prefix))

for root, dirnames, filenames in os.walk('src'):
    for filename in fnmatch.filter(filenames, '*.?pp'):
        path = os.path.join(root, filename)
        for match in re.findall(l9n_re, open(path).read()):
            if match not in all_ids:
                print path, "Error: can't find message for id '%s'" % match



