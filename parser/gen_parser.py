#!/usr/bin/python3
import re
from string import Template

with open('parser/bnf', 'r') as file:
  bnf = file.read()

bnf = bnf.replace('\\\n', '')

rules = re.findall(r"^\s*(\S*)\s*:\s*(.*)$", bnf, re.M)

tokens = set()
constants = set()
buffer = set()
rule_section = ''
decl_section = ''

for rule in rules:
  productions = rule[1].split('|')

  rule_section += '{0} : '.format(rule[0])

  is_first = True
  for production in productions:
    if not is_first:
      rule_section += '\t| '
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
        buffer.add('{0}List : {0} | {0}List {0}\n'.format(word))
      elif word.endswith('*'):
        word = word[:-1]
        rule_section += '{0}ListOptional '.format(word)
        buffer.add(
            '{0}ListOptional : %empty | {0}ListOptional {0}\n'.format(word))
      elif word.endswith('+,'):
        word = word[:-2]
        rule_section += '{0}CommaList '.format(word)
        buffer.add("{0}CommaList : {0} | {0}CommaList ',' {0}\n".format(word))
      elif word.endswith('*,'):
        word = word[:-2]
        rule_section += '{0}CommaListOptional '.format(word)
        buffer.add("{0}CommaList : {0} | {0}CommaList ',' {0}\n".format(word))
        buffer.add(
            '{0}CommaListOptional : %empty | {0}CommaList\n'.format(word))
      elif word.endswith('?'):
        word = word[:-1]
        rule_section += '{0}Optional '.format(word)
        buffer.add('{0}Optional : %empty | {0}\n'.format(word))
      else:
        rule_section += '{0} '.format(word)

      if word[:2] == 't_':
        tokens.add(word)
      elif word[:2] == 'c_':
        constants.add(word)

    rule_section += '\n'

  rule_section += '\n'

for r in buffer:
  rule_section += r

for token in tokens:
  decl_section += '%token {0}\n'.format(token)

for constant in constants:
  decl_section += '%token {0}\n'.format(constant)

with open('build/parser_gen.y', 'w') as parser, open('parser/parser_tmpl.y', 'r') as tmpl:
  t = Template(tmpl.read())
  parser.write(t.substitute(
      decl_section=decl_section, rule_section=rule_section))
