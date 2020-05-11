#!/usr/bin/python3
import re
from string import Template

with open('parser/bnf', 'r') as file:
  bnf = file.read()

bnf = bnf.replace('\\\n', '')

rules = re.findall(r"^\s*(\S*)\s*:\s*(.*)$", bnf, re.M)

tokens = set()
constants = set()
non_terminals = set()
buffer = set()
rule_section = ''
decl_section = ''

for rule in rules:
  productions = rule[1].split('|')

  rule_section += '{0} : '.format(rule[0])

  is_first = True
  for production in productions:
    if is_first:
      rule_section += '\n    '
    else:
      rule_section += '  | '
    is_first = False

    if production.split() == ['-', 'Expr']:
      rule_section += "'-' Expr %prec NEG\n"
      continue

    if production.split() == ['t_if', '(', 'Expr', ')', 'Stmt']:
      rule_section += "t_if '(' Expr ')' Stmt %prec THEN"
      continue

    for word in production.split():
      if len(word) < 2:
        rule_section += "'{0}' ".format(word)
      elif word.endswith('+'):
        word = word[:-1]
        rule_section += '{0}List '.format(word)
        buffer.add('{0}List : \n  {0} | {0}List {0}\n'.format(word))
        non_terminals.add('{0}List'.format(word))
      elif word.endswith('*'):
        word = word[:-1]
        rule_section += '{0}ListOptional '.format(word)
        buffer.add(
            '{0}ListOptional : \n  %empty\n  | {0}ListOptional {0}\n'.format(word))
        non_terminals.add('{0}List'.format(word))
      elif word.endswith('+,'):
        word = word[:-2]
        rule_section += '{0}CommaList '.format(word)
        buffer.add("{0}CommaList : \n  {0}\n  | {0}CommaList ',' {0}\n".format(word))
        non_terminals.add('{0}List'.format(word))
      elif word.endswith('*,'):
        word = word[:-2]
        rule_section += '{0}CommaListOptional '.format(word)
        buffer.add("{0}CommaList : \n  {0}\n  | {0}CommaList ',' {0}\n".format(word))
        buffer.add(
            '{0}CommaListOptional : \n  %empty\n  | {0}CommaList\n'.format(word))
        non_terminals.add('{0}List'.format(word))
      elif word.endswith('?'):
        word = word[:-1]
        rule_section += '{0}Optional '.format(word)
        buffer.add('{0}Optional : \n  %empty\n  | {0}\n'.format(word))
      else:
        rule_section += '{0} '.format(word)

      if word[:2] == 't_':
        tokens.add(word)
      elif word[:2] == 'c_':
        constants.add(word)
      elif word[0].isupper():
        non_terminals.add(word)

    rule_section += '\n'

  rule_section += ';\n\n'

for r in buffer:
  rule_section += r

for token in tokens:
  decl_section += '%token {0}\n'.format(token)
  
decl_section += '\n'

for constant in constants:
  decl_section += '%token {0}\n'.format(constant)

decl_section += '\n'

decl_union = set()

for non_terminal in non_terminals:
  decl_section += '%type <{0}_node_ptr_t> {0}\n'.format(non_terminal)
  decl_union.add('\t{0}_node *{0}_node_ptr_t;\n'.format(non_terminal))

decl_section += '\n%union {\n\tast_node_ptr_t base_node_ptr_t;\n'
for field in decl_union:
  decl_section += field
decl_section += '}\n'

with open('build/parser_gen.yxx', 'w') as parser, open('parser/parser_tmpl.yxx', 'r') as tmpl:
  t = Template(tmpl.read())
  parser.write(t.substitute(
      decl_section=decl_section, rule_section=rule_section))
