import os
import sys
import subprocess
sys.path.insert(0, os.path.abspath('.'))

HOME_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
HOME_DIR = os.path.abspath(HOME_DIR)
print("HOME_DIR: " + HOME_DIR)

def configureDoxyfile(input_dir, output_dir):
    with open('Doxyfile.in', 'r') as file :
        filedata = file.read()

    filedata = filedata.replace('@DOXYGEN_INPUT_DIR@', input_dir)
    filedata = filedata.replace('@DOXYGEN_OUTPUT_DIR@', output_dir)

    with open('Doxyfile', 'w') as file:
        file.write(filedata)

# Check if we're running on Read the Docs' servers
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

breathe_projects = { "bayesmix": "../build/docs/docs/doxygen/xml " }
breathe_default_project = "bayesmix"


if read_the_docs_build:
    input_dir = '../src'
    output_dir = 'build'
    configureDoxyfile(input_dir, output_dir)
    subprocess.call('doxygen', shell=True)
    breathe_projects['bayesmix'] = output_dir + '/xml'
    protoc_gen_cmd = "docker run --rm --platform linux/amd64 " + \
        "-v {0}/docs:/out -v {0}/proto:/protos ".format(HOME_DIR) + \
        "pseudomuto/protoc-gen-doc --doc_opt=html,protos.html"
    subprocess.call(protoc_gen_cmd.split(" "))

project = 'bayesmix'
copyright = '2021, Guindani, B. and Beraha, M.'
author = 'Guindani, B. and Beraha, M.'

# The full version, including alpha/beta/rc tags
release = '0.0.1'

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.doctest',
    'sphinx.ext.mathjax',
    'sphinx.ext.viewcode',
    'sphinx.ext.imgmath',
    'sphinx.ext.todo',
    'breathe',
]


templates_path = ['_templates']

exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

html_theme = 'haiku'

html_static_path = ['_static']

highlight_language = 'cpp'

imgmath_latex = 'latex'
