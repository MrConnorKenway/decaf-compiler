#!/usr/bin/python3
import re
import argparse
from string import Template

'''
Python script used for automatically generating
virtual function declaration for Visitor class.
'''


def gen_base(args):
  with open('visitor/visitor_tmpl.h', 'r') as vis_tmpl, \
          open('ast/ast_tmpl.h', 'r') as ast_tmpl, \
          open(args.header, 'w') as vis:
    ast_tmpl = ast_tmpl.read()
    subclass_names = re.findall(
        r"class (.*) : .* {[\s\S]*?};", ast_tmpl, re.MULTILINE)
    vis_tmpl = Template(vis_tmpl.read().replace('//', ''))
    function_decl = ''
    for subclass_name in subclass_names:
      function_decl += '\n\tvirtual void visit({0}*) = 0;'.format(
          subclass_name)
    vis.write(vis_tmpl.substitute(decl=function_decl))


def gen(args):
  # function to generate arbitrary visitor subclass
  with open(args.src, 'w') as src, \
          open('visitor/visitor_tmpl.h', 'r') as vis, \
          open('visitor/'+args.class_name+'_tmpl.cxx', 'r') as src_tmpl, \
          open('visitor/'+args.class_name+'_tmpl.h', 'r') as header_tmpl, \
          open('ast/ast_tmpl.h', 'r') as ast_tmpl, \
          open(args.header, 'w') as header:
    class_name = args.class_name
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
      function_def += '\nvoid {0}::visit({1}* {2}) {{\n}}\n'.format(
          class_name, subclass_name, subclass_name.lower() + '_ptr')
    header.write(header_tmpl.substitute(decl=function_decl))
    src.write(src_tmpl.substitute(decl=function_def))


if __name__ == "__main__":
  parser = argparse.ArgumentParser(description='Generate visitor class')
  parser.add_argument('class_name', help='Class name to be generated')
  parser.add_argument('--src-dst', dest='src',
                      help='specify the destination of generated source file')
  parser.add_argument('--header-dst', dest='header',
                      help='specify the destination of generated header file')

  args = parser.parse_args()
  if args.class_name == 'base':
    assert args.header != None
    gen_base(args)
  else:
    assert args.header != None
    assert args.src != None
    gen(args)
