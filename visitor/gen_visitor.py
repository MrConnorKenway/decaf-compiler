#!/usr/bin/python3
import re
import sys
from string import Template

'''
Python script used for automatically generating
virtual function declaration for Visitor class.
'''


def gen_base():
  with open('visitor/visitor_tmpl.h', 'r') as vis_tmpl, \
          open('ast/ast_tmpl.h', 'r') as ast_tmpl, \
          open('visitor/visitor.h', 'w') as vis:
    ast_tmpl = ast_tmpl.read()
    subclass_names = re.findall(
        r"class (.*) : .* {[\s\S]*?};", ast_tmpl, re.MULTILINE)
    vis_tmpl = Template(vis_tmpl.read().replace('//', ''))
    function_decl = ''
    for subclass_name in subclass_names:
      function_decl += '\n\tvirtual void visit({0}*) = 0;'.format(
          subclass_name)
    vis.write(vis_tmpl.substitute(decl=function_decl))


def gen(class_name):
  # function to generate arbitrary visitor subclass
  dir = 'visitor/'
  with open(dir+class_name+'.cxx', 'w') as src, \
          open('visitor/visitor_tmpl.h', 'r') as vis, \
          open(dir+class_name+'_tmpl.cxx', 'r') as src_tmpl, \
          open(dir+class_name+'_tmpl.h', 'r') as header_tmpl, \
          open('ast/ast_tmpl.h', 'r') as ast_tmpl, \
          open(dir+class_name+'.h', 'w') as header:
    assert class_name != 'visitor'
    class_name = class_name[0].upper() + class_name[1:]
    src_tmpl = Template(src_tmpl.read().replace('//', ''))
    header_tmpl = Template(header_tmpl.read().replace('///', ''))

    ast_tmpl = ast_tmpl.read()
    subclass_names = re.findall(
        r"class (.*) : .* {[\s\S]*?};", ast_tmpl, re.MULTILINE)

    function_decl = ''
    function_def = ''
    for subclass_name in subclass_names:
      function_decl += '\n\t/*virtual*/ void visit({0}*);'.format(
          subclass_name)
      function_def += '\nvoid {0}::visit({1}* {2}) {{\n\tcout << {2}->node_type;\n}}\n'.format(
          class_name, subclass_name, subclass_name.lower() + '_ptr')
    header.write(header_tmpl.substitute(decl=function_decl))
    src.write(src_tmpl.substitute(decl=function_def))


if __name__ == "__main__":
  if len(sys.argv) == 1:
    gen_base()
  else:
    assert len(sys.argv) == 2
    gen(sys.argv[1])
