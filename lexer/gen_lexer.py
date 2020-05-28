#!/usr/bin/python3
import re
import argparse
from string import Template

parser = argparse.ArgumentParser(
    description='Generate flex file')
parser.add_argument(
    'l_dst', help='specify the destination of generated flex file')

args = parser.parse_args()

rule_list = ''
with open('lexer/keywords', 'r') as keywords, \
        open('lexer/operators', 'r') as operators, \
        open('lexer/lexer_tmpl.l', 'r') as tmpl, \
        open(args.l_dst, 'w') as lexer_gen:

  keywords = keywords.read().replace('\n', ' ').split()
  for keyword in keywords:
    rule_list += '\"{0}\" {{\n\tyylval.base_node_ptr_t = new Empty_node();\n\tyylval.base_node_ptr_t->yylloc_ptr = new YYLTYPE(yylloc);\n\treturn t_{0};\n}}\n'.format(keyword)

  operators = operators.read().split('\n')
  for operator in operators:
    name, operator = operator.split()
    if len(operator) == 1:
      rule_list += "\"{0}\" {{ return '{0}'; }}\n".format(operator)
    else:
      rule_list += "\"{0}\" {{ return t_{1}; }}\n".format(operator, name)

  t = Template(tmpl.read())
  lexer_gen.write(t.substitute(rule_list=rule_list))
