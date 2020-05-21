#
# Makefile fragment for decaf-compiler.
# This is NOT a complete makefile;
# you must run GNU make in the top-level directory.
#

OBJ += $(OBJDIR)/main.o

$(OBJDIR)/main.o: kern/main.cxx $(INCDIR)/display_visitor.h
	@echo - generating $(@F)
	@g++ -c -o $@ $< $(CFLAGS)