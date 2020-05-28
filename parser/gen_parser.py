#!/usr/bin/python3
import re
import argparse
from string import Template

with open('parser/bnf', 'r') as file:
  bnf = file.read()

bnf = bnf.replace('\\\n', '')

rules = re.findall(r"^\s*(\S*)\s*:\s*(.*)$", bnf, re.M)

# use set to avoid duplication
tokens = set()
non_terminals = set()
# buffer: temporally store the string content
buffer = set()
rule_section = ''
decl_section = ''

for rule in rules:
  productions = rule[1].split('|')

  # rule[0]: the LHS of a rule
  # rule[1]: the RHS of a rule
  rule_section += '{0} : '.format(rule[0])
  non_terminals.add(rule[0])

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
      rule_section += "t_if '(' Expr ')' Stmt %prec THEN\n"
      continue

    for word in production.split():
      if len(word) < 2:
        rule_section += "'{0}' ".format(word)
      elif word.endswith('+'):
        word = word[:-1]
        rule_section += '{0}List '.format(word)
        buffer.add('{0}List: \n    {0}\n | {0}List {0}\n;\n\n'.format(word))
        non_terminals.add('{0}List'.format(word))
      elif word.endswith('*'):
        word = word[:-1]
        rule_section += '{0}ListOptional '.format(word)
        buffer.add(
            '{0}ListOptional: \n    %empty\n  | {0}List\n;\n\n'.format(word))
        buffer.add('{0}List: \n    {0}\n | {0}List {0}\n;\n\n'.format(word))
        non_terminals.add('{0}ListOptional'.format(word))
        non_terminals.add('{0}List'.format(word))
      elif word.endswith('+,'):
        word = word[:-2]
        rule_section += '{0}CommaList '.format(word)
        buffer.add(
            "{0}CommaList: \n    {0}\n  | {0}CommaList ',' {0}\n;\n\n".format(word))
        non_terminals.add('{0}CommaList'.format(word))
      elif word.endswith('*,'):
        word = word[:-2]
        rule_section += '{0}CommaListOptional '.format(word)
        buffer.add(
            "{0}CommaList: \n    {0}\n  | {0}CommaList ',' {0}\n;\n\n".format(word))
        buffer.add(
            '{0}CommaListOptional: \n    %empty\n  | {0}CommaList\n;\n\n'.format(word))
        non_terminals.add('{0}CommaListOptional'.format(word))
        non_terminals.add('{0}CommaList'.format(word))
      elif word.endswith('?'):
        word = word[:-1]
        rule_section += '{0}Optional '.format(word)
        buffer.add('{0}Optional: \n    %empty\n  | {0}\n;\n\n'.format(word))
        non_terminals.add('{0}Optional'.format(word))
      else:
        rule_section += '{0} '.format(word)

      if word[:2] == 't_':
        tokens.add(word)
      elif word[0].isupper():
        non_terminals.add(word)

    rule_section += '\n'

  rule_section += ';\n\n'

for r in buffer:
  rule_section += r

for token in tokens:
  if token == 't_ident':
    decl_section += '%token <Ident_node_ptr_t> {0}\n'.format(token)
  elif token == 't_type_ident':
    decl_section += '%token <User_defined_type_node_ptr_t> {0}\n'.format(token)
  else:
    decl_section += '%token <base_node_ptr_t> {0}\n'.format(token)

decl_section += '\n'

decl_union = set()

# We want some node to be base_node_ptr_t
with open('parser/parser_tmpl.yxx', 'r') as tmpl:
  base_node = re.findall(r"%type <base_node_ptr_t> (.*)", tmpl.read(), re.M)
  for t in base_node:
    non_terminals.discard(t)

for non_terminal in non_terminals:
  if non_terminal.find('List') >= 0 or non_terminal.find('Implementer') >= 0:
    # treat all List as the same
    decl_section += '%type <List_node_ptr_t> {0}\n'.format(non_terminal)
    decl_union.add('\tList_node *List_node_ptr_t;\n')
  else:
    # we don't want non_terminal which contains 'Optional' to introduce
    # new node type
    beg = non_terminal.find('Optional')
    if beg >= 0:
      decl_section += '%type <base_node_ptr_t> {0}\n'.format(non_terminal)
    else:
      decl_section += '%type <{0}_node_ptr_t> {0}\n'.format(non_terminal)
      decl_union.add('\t{0}_node* {0}_node_ptr_t;\n'.format(non_terminal))

union_section = ''
for field in decl_union:
  union_section += field

parser = argparse.ArgumentParser(
    description='Generate bison file')
parser.add_argument(
    'yxx_dst', help='specify the destination of generated bison file')

args = parser.parse_args()

with open(args.yxx_dst, 'w') as parser, open('parser/parser_tmpl.yxx', 'r') as tmpl:
  t = Template(tmpl.read())
  parser.write(t.substitute(
      decl_section=decl_section, rule_section=rule_section, union_section=union_section))
