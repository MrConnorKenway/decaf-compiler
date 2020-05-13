#!/usr/bin/python3
import re
from string import Template

'''
Python script used to generating constructor function and 
other neccessary functions of ast node according to template file
'''

with open('ast/ast.h', 'w') as header, open('ast/ast_tmpl.h', 'r') as tmpl, open('ast/ast.cxx', 'w') as src:
  tmpl = tmpl.read()
  # use special mark to find the end of directly copied part
  beg = tmpl.find('// $')
  header.write(tmpl[:beg])
  subclass_names = re.findall(
      r"class (.*) : .* {[\s\S]*?};", tmpl, re.MULTILINE)

  for subclass_name in subclass_names:
    header.write('class {0};\n'.format(subclass_name))

  header.write('\n')

  for subclass_name in subclass_names:
    fields_decl = re.findall(
        r"class {0}.*\n^.*public:.*\n\s*([\s\S]*?)}};".format(subclass_name), tmpl, re.MULTILINE)

    if len(fields_decl) == 0:
      # if the class does not have any fields
      header.write(
          'class {0} : public AST_node_base {{\n public:\n\t/*virtual*/ void accept(Visitor& v);\n}};\n\n'.format(subclass_name))
    elif subclass_name == 'List_node':
      # we want List_node use default constructor
      header.write(
          'class {0} : public AST_node_base {{\n public:\n\t{1}\n\t/*virtual*/ void accept(Visitor& v);\n\t{0}();\n}};\n\n'.format(
              subclass_name, fields_decl[0]))
    else:
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
      header.write(
          'class {0} : public AST_node_base {{\n public:\n\t{3}\n\t/*virtual*/ void accept(Visitor& v);\n\t{0}({1});\n}};\n\n'.format(
              subclass_name, parameter_list_str, assignment_list_str, fields_decl[0]))

  src.write('#include "../ast/ast.h"\n#include "../visitor/visitor.h"\n\n')

  for subclass_name in subclass_names:
    fields_decl = re.findall(
        r"class {0}.*\n^.*public:.*\n\s*([\s\S]*?)}};".format(subclass_name), tmpl, re.MULTILINE)

    if len(fields_decl) == 0:
      # if the class does not have any fields
      src.write(
          'void {0}::accept(Visitor& v) {{ v.visit(this); }}\n\n'.format(subclass_name))
    elif subclass_name == 'List_node':
      # we want List_node use default constructor
      src.write(
          'void {0}::accept(Visitor& v) {{ v.visit(this); }}\n{0}::{0}() {{ node_type = "{0}"; }}\n\n'.format(subclass_name))
    else:
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
      src.write(
          'void {0}::accept(Visitor& v) {{ v.visit(this); }}\n{0}::{0}({1}) : {2} {{ node_type = "{0}"; }}\n\n'.format(subclass_name, parameter_list_str, assignment_list_str, fields_decl[0]))
