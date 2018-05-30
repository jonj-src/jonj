jonjdbg: $(BUILD_BINARY)

jonjdbg-shared: $(BUILD_SHARED)

$(BUILD_SHARED): $(JONJ_GLOBAL_OBJS) $(JONJ_BINARY_OBJS) $(JONJ_JONJDBG_OBJS)
	$(BUILD_JONJDBG_SHARED)

$(BUILD_BINARY): $(JONJ_GLOBAL_OBJS) $(JONJ_BINARY_OBJS) $(JONJ_JONJDBG_OBJS)
	$(BUILD_JONJDBG)

%.c: %.y
%.c: %.l

$(builddir)/jonjdbg_lexer.lo: $(srcdir)/jonjdbg_parser.h

$(srcdir)/jonjdbg_lexer.c: $(srcdir)/jonjdbg_lexer.l
	@(cd $(top_srcdir); $(RE2C) $(RE2C_FLAGS) --no-generation-date -cbdFo $(srcdir)/jonjdbg_lexer.c $(srcdir)/jonjdbg_lexer.l)

$(srcdir)/jonjdbg_parser.h: $(srcdir)/jonjdbg_parser.c
$(srcdir)/jonjdbg_parser.c: $(srcdir)/jonjdbg_parser.y
	@$(YACC) -p jonjdbg_ -v -d $(srcdir)/jonjdbg_parser.y -o $@

install-jonjdbg: $(BUILD_BINARY)
	@echo "Installing jonjdbg binary:         $(INSTALL_ROOT)$(bindir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(bindir)
	@$(mkinstalldirs) $(INSTALL_ROOT)$(localstatedir)/log
	@$(mkinstalldirs) $(INSTALL_ROOT)$(localstatedir)/run
	@$(INSTALL) -m 0755 $(BUILD_BINARY) $(INSTALL_ROOT)$(bindir)/$(program_prefix)jonjdbg$(program_suffix)$(EXEEXT)
	@echo "Installing jonjdbg man page:       $(INSTALL_ROOT)$(mandir)/man1/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(mandir)/man1
	@$(INSTALL_DATA) $(srcdir)/jonjdbg.1 $(INSTALL_ROOT)$(mandir)/man1/$(program_prefix)jonjdbg$(program_suffix).1

clean-jonjdbg:
	@echo "Cleaning jonjdbg object files ..."
	find sapi/jonjdbg/ -name *.lo -o -name *.o | xargs rm -f

test-jonjdbg:
	@echo "Running jonjdbg tests ..."
	@$(top_builddir)/sapi/cli/jonj sapi/jonjdbg/tests/run-tests.jonj --jonjdbg sapi/jonjdbg/jonjdbg

.PHONY: clean-jonjdbg test-jonjdbg

