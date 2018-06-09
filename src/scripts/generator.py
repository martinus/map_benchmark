import cgi
import os
import json
import glob

from pprint import pprint
from jinja2 import Environment, FileSystemLoader

THIS_DIR = os.path.dirname(os.path.abspath(__file__))


def parse():
    data = []
    for file in glob.glob('*.json'):
        with open(file) as f:
            data.append(json.load(f))

    return data


def print_html_doc(mem_data):
    # Create the jinja2 environment.
    # Notice the use of trim_blocks, which greatly helps control whitespace.
    j2_env = Environment(loader=FileSystemLoader(THIS_DIR), trim_blocks=True)

    print j2_env.get_template('test_template.html.j2').render(
        title=cgi.escape('TODO'),
        mem_data=mem_data)


if __name__ == '__main__':
    data = parse()
    print_html_doc(data)
