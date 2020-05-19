#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

OBJ += $(OBJDIR)/parser.o

$(OBJDIR)/parser.o: $(SRCDIR)/parser_handwritten.tab.cxx $(INCDIR)/ast.h
	@echo - generating $(@F)
	@g++ -c -o $@ $< -ly $(CFLAGS)

$(OBJDIR)/%_main.o: parser/parser_%_main.cxx $(OBJDIR)/%.o
	@echo - generating $(@F)
	@g++ -c -o $@ $< $(CFLAGS)

$(GENDIR)/parser_gen.yxx: parser/gen_parser.py parser/bnf parser/parser_tmpl.yxx
	@echo - generating bison file
	@$< $@

parser/parser_handwritten.yxx: parser/merge_hand_and_gen.py $(GENDIR)/parser_gen.yxx
	@echo - init parser_handwritten.yxx using parser_gen.yxx
	@$< $(word 2, $^)

$(SRCDIR)/parser_handwritten.tab.cxx: parser/parser_handwritten.yxx
	@echo - using handwritten bison file
	@echo - generating parser file
	@bison --defines=$(INCDIR)/parser_handwritten.tab.h $< -v -Wall -o $@

$(INCDIR)/parser_handwritten.tab.h: parser/parser_handwritten.yxx
	@echo - using handwritten bison file
	@echo - generating parser file
	@bison --defines=$@ $< -v -Wall -o $(SRCDIR)/parser_handwritten.tab.cxx

.DELETE_ON_ERROR: $(GENDIR)/parser_gen.yxx