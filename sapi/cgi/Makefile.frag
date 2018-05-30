cgi: $(SAPI_CGI_PATH)

$(SAPI_CGI_PATH): $(JONJ_GLOBAL_OBJS) $(JONJ_BINARY_OBJS) $(JONJ_CGI_OBJS)
	$(BUILD_CGI)

install-cgi: $(SAPI_CGI_PATH)
	@echo "Installing JONJ CGI binary:        $(INSTALL_ROOT)$(bindir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(bindir)
	@$(INSTALL) -m 0755 $(SAPI_CGI_PATH) $(INSTALL_ROOT)$(bindir)/$(program_prefix)jonj-cgi$(program_suffix)$(EXEEXT)
	@echo "Installing JONJ CGI man page:      $(INSTALL_ROOT)$(mandir)/man1/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(mandir)/man1
	@$(INSTALL_DATA) sapi/cgi/jonj-cgi.1 $(INSTALL_ROOT)$(mandir)/man1/$(program_prefix)jonj-cgi$(program_suffix).1

