#!/usr/bin/python3
import argparse

'''
When writing the syntax action of bison rule list,
I frequently need to generate parser_handwritten.yxx.
This script is used for auto merging the handwritten part
and machine generated part.
'''

parser = argparse.ArgumentParser(
    description='Merge machine generated code and handwritten code')
parser.add_argument(
    'src', help='specify the destination of source yxx file')

args = parser.parse_args()

with open(args.src, 'r') as gen_src, open('parser/parser_handwritten.yxx', 'r') as handwritten_src:
  gen_src = gen_src.read()
  handwritten_src = handwritten_src.read()

  handwritten_src = gen_src[:gen_src.find(
      '%%')] + handwritten_src[handwritten_src.find('%%'):]

with open('parser/parser_handwritten.yxx', 'w') as dst:
  dst.write(handwritten_src)
