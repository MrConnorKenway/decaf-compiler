#!/usr/bin/python3
import re
from string import Template

'''
Python script used to generating constructor function and 
other neccessary functions of ast node according to template file
'''

with open('ast/ast.h', 'w') as ast, open('ast/ast_tmpl.h', 'r') as tmpl:
  tmpl = tmpl.read()
  # use special mark to find the end of directly copied part
  ast.write(tmpl[:tmpl.find('// $')])
  subclass_names = re.findall(
      r"class (.*) : .* {[\s\S]*?};", tmpl, re.MULTILINE)
  for subclass_name in subclass_names:
    fields_decl = re.findall(
        r"class {0}.*\n^.*public:.*\n\s*([\s\S]*?)}};".format(subclass_name), tmpl, re.MULTILINE)

    # only generate non_terminal class that is in template file
    if len(fields_decl) == 0:
      continue
    fields = fields_decl[0][:-2].replace('\n', '').split(';')
    parameter_list_str = ''
    assignment_list_str = ''
    is_first = True
    for field in fields:
      type_name, id_name = field.split()
      if is_first:
        parameter_list_str += '{0} _{1}'.format(type_name, id_name)
        assignment_list_str += '{0}(_{0})'.format(id_name)
        is_first = False
      else:
        parameter_list_str += ', {0} _{1}'.format(type_name, id_name)
        assignment_list_str += ', {0}(_{0})'.format(id_name)
    ast.write(
        'class {0} : public AST_node_base {{\n public:\n\t{3}\n\t/*virtual*/ void accept(Visitor& v) {{ v.visit(this); }}\n\t{0}({1}) : {2} {{ node_type = \"{0}\"; }};\n}};\n\n'.format(
            subclass_name, parameter_list_str, assignment_list_str, fields_decl[0]))
