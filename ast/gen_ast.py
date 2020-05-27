#!/usr/bin/python3
import re
import argparse
from string import Template

'''
Python script used to generating constructor function and 
other neccessary functions of ast node according to template file
'''


def main(src, header, tmpl):
  tmpl = tmpl.read()
  # use special mark to find the end of directly copied part
  beg = tmpl.find('// $')
  header.write(tmpl[:beg])
  subclass_names = re.findall(
      r"class (.*) : .* {[\s\S]*?};", tmpl, re.MULTILINE)

  for subclass_name in subclass_names:
    header.write('class {0};\n'.format(subclass_name))

  header.write('\n')
  src.write('#include "build/include/ast.h"\n#include "build/include/visitor.h"\n\n')

  for subclass_name in subclass_names:
    fields_decl = re.findall(
        r"class {0}.*\n^.*public:.*\n\s*([\s\S]*?)}};".format(subclass_name), tmpl, re.MULTILINE)

    if len(fields_decl) == 0:
      # if the class does not have any fields
      header.write(
          'class {0} : public AST_node_base {{\n public:\n\t/*virtual*/ void accept(Visitor& v);\n\t{0}();\n}};\n\n'.format(subclass_name))
      src.write(
          'void {0}::accept(Visitor& v) {{ v.visit(this); }}\n{0}::{0}() {{ node_type = "{0}"; }}\n\n'.format(subclass_name))
    else:
      fields = re.findall(r"([\s\S]*?)\n", fields_decl[0], re.MULTILINE)
      parameter_list_str = ''
      assignment_list_str = ''
      is_first = True
      for field in fields:
        field = field.replace(';', '')
        if field.count('//') > 0:
          # if we specify the initial value of this field
          field, init_value = field.split('//')
          type_name, id_name = field.split()
          init_value = init_value.split()[0]
          if init_value == 'default':
            init_value = ''
          if is_first:
            assignment_list_str += '{0}({1})'.format(id_name, init_value)
            is_first = False
          else:
            assignment_list_str += ', {0}({1})'.format(id_name, init_value)
        else:
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
      src.write(
          'void {0}::accept(Visitor& v) {{ v.visit(this); }}\n{0}::{0}({1}) : {2} {{ node_type = "{0}"; }}\n\n'.format(subclass_name, parameter_list_str, assignment_list_str, fields_decl[0]))


if __name__ == "__main__":
  parser = argparse.ArgumentParser(
      description='Generate ast src and header file')
  parser.add_argument('--src-dst', dest='src',
                      help='specify the destination of generated source file')
  parser.add_argument('--header-dst', dest='header',
                      help='specify the destination of generated header file')

  args = parser.parse_args()

  with open(args.header, 'w') as header, open('ast/ast_tmpl.h', 'r') as tmpl, open(args.src, 'w') as src:
    main(src, header, tmpl)
