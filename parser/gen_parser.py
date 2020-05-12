#!/usr/bin/python3
import re
from string import Template

with open('parser/bnf', 'r') as file:
  bnf = file.read()

bnf = bnf.replace('\\\n', '')

rules = re.findall(r"^\s*(\S*)\s*:\s*(.*)$", bnf, re.M)

# use set to avoid duplication
tokens = set()
constants = set()
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
        non_terminals.add('{0}ListOptional'.format(word))
      elif word.endswith('+,'):
        word = word[:-2]
        rule_section += '{0}CommaList '.format(word)
        buffer.add(
            "{0}CommaList : \n  {0}\n  | {0}CommaList ',' {0}\n".format(word))
        non_terminals.add('{0}ListCommaList'.format(word))
      elif word.endswith('*,'):
        word = word[:-2]
        rule_section += '{0}CommaListOptional '.format(word)
        buffer.add(
            "{0}CommaList : \n  {0}\n  | {0}CommaList ',' {0}\n".format(word))
        buffer.add(
            '{0}CommaListOptional : \n  %empty\n  | {0}CommaList\n'.format(word))
        non_terminals.add('{0}CommaListOptional'.format(word))
      elif word.endswith('?'):
        word = word[:-1]
        rule_section += '{0}Optional '.format(word)
        buffer.add('{0}Optional : \n  %empty\n  | {0}\n'.format(word))
        non_terminals.add('{0}Optional'.format(word))
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
  decl_section += '%token <int_val> {0}\n'.format(token)

decl_section += '\n'

for constant in constants:
  decl_section += '%token <int_val> {0}\n'.format(constant)

decl_section += '\n'

decl_union = set()

# We want some node to be base_node_ptr_t
with open('parser/parser_tmpl.yxx', 'r') as tmpl:
  base_node = re.findall(r"%type <.*> (.*)", tmpl.read(), re.M)
  for t in base_node:
    non_terminals.discard(t)

with open('build/ast.h', 'w') as ast, open('parser/ast_tmpl.h', 'r') as tmpl:
  tmpl = tmpl.read()
  ast.write(tmpl[:tmpl.find('//')])
  for non_terminal in non_terminals:
    if non_terminal.find('List') >= 0:
      # treat all List as the same
      decl_section += '%type <List_node_ptr_t> {0}\n'.format(non_terminal)
      decl_union.add('\tList_node *List_node_ptr_t;\n')
    else:
      # we don't want non_terminal which contains 'Optional' to introduce
      # new node type
      beg = non_terminal.find('Optional')
      if beg >= 0:
        decl_section += '%type <{0}_node_ptr_t> {1}\n'.format(
            non_terminal[:beg], non_terminal)
      else:
        decl_section += '%type <{0}_node_ptr_t> {0}\n'.format(non_terminal)
        decl_union.add('\t{0}_node *{0}_node_ptr_t;\n'.format(non_terminal))
        fields_decl = re.findall(
            r"class {0}_node.*\n^.*public:.*\n\s*([\s\S]*?)}};".format(non_terminal), tmpl, re.MULTILINE)

        # only generate class that is in template file
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
            'class {0}_node : public AST_node_base {{\n public:\n\t{3}\n\t{0}_node({1}) : {2} {{ node_type = \"{0}_node\"; }};\n}};\n\n'.format(
                non_terminal, parameter_list_str, assignment_list_str, fields_decl[0]))

union_section = '\n%union {\n\tast_node_ptr_t base_node_ptr_t;\n'
for field in decl_union:
  union_section += field
decl_section = union_section + '}\n\n' + decl_section

with open('build/parser_gen.yxx', 'w') as parser, open('parser/parser_tmpl.yxx', 'r') as tmpl:
  t = Template(tmpl.read())
  parser.write(t.substitute(
      decl_section=decl_section, rule_section=rule_section))
