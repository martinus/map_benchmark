import os

from jinja2 import Environment, FileSystemLoader

THIS_DIR = os.path.dirname(os.path.abspath(__file__))


def print_html_doc():
    # Create the jinja2 environment.
    # Notice the use of trim_blocks, which greatly helps control whitespace.
    j2_env = Environment(loader=FileSystemLoader(THIS_DIR), trim_blocks=True)

    print j2_env.get_template('test_template.html.j2').render(
        title='Hellow Gist from GutHub')


if __name__ == '__main__':
    print_html_doc()
