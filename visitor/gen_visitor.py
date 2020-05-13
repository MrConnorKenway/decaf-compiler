#!/usr/bin/python3
import re
from string import Template

'''
Python script used for automatically generating
virtual function declaration for Visitor class.
'''

with open('visitor/visitor_tmpl.h', 'r') as vis_tmpl, open('ast/ast_tmpl.h', 'r') as ast_tmpl, open('visitor/visitor.h', 'w') as vis:
  ast_tmpl = ast_tmpl.read()
  subclass_names = re.findall(
      r"class (.*) : .* {[\s\S]*?};", ast_tmpl, re.MULTILINE)
  vis_tmpl = Template(vis_tmpl.read().replace('//', ''))
  function_decl = ''
  for subclass_name in subclass_names:
    function_decl += '\n\tvirtual void visit({0}*) = 0;'.format(
        subclass_name)
  vis.write(vis_tmpl.substitute(decl=function_decl))
