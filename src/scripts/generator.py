#!/usr/bin/env python
import html
import os
import json
import glob

from collections import defaultdict
from pprint import pprint
from jinja2 import Environment, FileSystemLoader

THIS_DIR = os.path.dirname(os.path.abspath(__file__))


def parse():
    data = defaultdict(list)
    for file in sorted(glob.glob('*.json')):
        testname = file[:file.find('_')]

        with open(file) as f:
            data[testname].append(json.load(f))

    return data


def print_html_doc(mem_data):
    # Create the jinja2 environment.
    # Notice the use of trim_blocks, which greatly helps control whitespace.
    j2_env = Environment(loader=FileSystemLoader(THIS_DIR), trim_blocks=True)

    tpl = j2_env.get_template('test_template.html.j2')

    for testname in mem_data:
        with open(testname + '.html', 'w') as out_file:
            print('writing "' + testname + '.html"')
            out_file.write(
                tpl.render(
                    title=html.escape(testname),
                    mem_data=mem_data[testname]))


if __name__ == '__main__':
    data = parse()
    print_html_doc(data)
