#!/usr/bin/python3
import re
from string import Template

rule_list = ''
with open('lexer/keywords', 'r') as keywords, \
        open('lexer/operators', 'r') as operators, \
        open('lexer/lexer_tmpl.l', 'r') as tmpl, \
        open('build/lexer_gen.l', 'w') as lexer_gen:

  keywords = keywords.read().replace('\n', ' ').split()
  for keyword in keywords:
    rule_list += '{0} {{\n\treturn t_{0};\n}}\n'.format(keyword)

  operators = operators.read().split('\n')
  for operator in operators:
    name, operator = operator.split()
    if len(operator) == 1:
      rule_list += "\"{0}\" {{\n\treturn '{0}';\n}}\n".format(operator)
    else:
      rule_list += "\"{0}\" {{\n\treturn t_{1};\n}}\n".format(operator, name)

  t = Template(tmpl.read())
  lexer_gen.write(t.substitute(rule_list=rule_list))
