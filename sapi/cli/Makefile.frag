cli: $(SAPI_CLI_PATH)

$(SAPI_CLI_PATH): $(JONJ_GLOBAL_OBJS) $(JONJ_BINARY_OBJS) $(JONJ_CLI_OBJS)
	$(BUILD_CLI)

install-cli: $(SAPI_CLI_PATH)
	@echo "Installing JONJ CLI binary:        $(INSTALL_ROOT)$(bindir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(bindir)
	@$(INSTALL) -m 0755 $(SAPI_CLI_PATH) $(INSTALL_ROOT)$(bindir)/$(program_prefix)jonj$(program_suffix)$(EXEEXT)
	@echo "Installing JONJ CLI man page:      $(INSTALL_ROOT)$(mandir)/man1/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(mandir)/man1
	@$(INSTALL_DATA) sapi/cli/jonj.1 $(INSTALL_ROOT)$(mandir)/man1/$(program_prefix)jonj$(program_suffix).1

