
import codecs
import os.path
from xml.etree.ElementTree import parse
from StringIO import StringIO
import fnmatch
import os
import re

wt_messages = parse(open("wt.xml")).getroot()
wt_ids = set()
for message in wt_messages:
    wt_ids.add(message.get('id'))

sections = ['common', 'competition', 'game', 'time', 'user']

Ids = set()

for filename in os.listdir('locales'):
    path = os.path.join('locales', filename)
    xml_file = codecs.open(path, 'r', 'utf-8')
    assert not xml_file.read().count('\t')
    xml_file.seek(0)
    for line in xml_file:
        assert len(line) <= 120
    xml_file = StringIO(open(path).read().replace('&',''))
    messages = parse(xml_file).getroot()
    keys = set()
    for message in messages:
        assert message.tag == 'message'
        Id = message.get('id')
        Ids.add(Id)
        text = message.text
        assert not text or '\n' in text or (text[0] != ' ' and text[-1] != ' ')
        assert Id not in keys
        keys.add(Id)
        assert Id.startswith('Wt.') or Id.startswith('tc.')
        if Id.startswith('Wt.'):
            assert Id in wt_ids
        else:
            tc, section, id = Id.split('.')
            assert section in sections
            assert '-' not in id
            assert not text or id[0].islower() == text[0].islower() or not text[0].isalpha()
            if '${' in text:
                assert id.endswith('_template')
    short_messages = [m.get('id').lower() for m in messages if m.text and '\n' not in m.text and m.get('id').startswith('tc.')]
    assert short_messages == sorted(short_messages)

l9n_re = re.compile(r'"(tc\.[^"]+)"')

for root, dirnames, filenames in os.walk('src'):
    for filename in fnmatch.filter(filenames, '*.?pp'):
        path = os.path.join(root, filename)
        for match in re.findall(l9n_re, open(path).read()):
            assert match in Ids


