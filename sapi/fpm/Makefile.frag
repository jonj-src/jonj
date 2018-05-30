fpm: $(SAPI_FPM_PATH)

$(SAPI_FPM_PATH): $(JONJ_GLOBAL_OBJS) $(JONJ_BINARY_OBJS) $(JONJ_FPM_OBJS)
	$(BUILD_FPM)

install-fpm: $(SAPI_FPM_PATH)
	@echo "Installing JONJ FPM binary:        $(INSTALL_ROOT)$(sbindir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(sbindir)
	@$(mkinstalldirs) $(INSTALL_ROOT)$(localstatedir)/log
	@$(mkinstalldirs) $(INSTALL_ROOT)$(localstatedir)/run
	@$(INSTALL) -m 0755 $(SAPI_FPM_PATH) $(INSTALL_ROOT)$(sbindir)/$(program_prefix)jonj-fpm$(program_suffix)$(EXEEXT)

	@echo "Installing JONJ FPM config:        $(INSTALL_ROOT)$(sysconfdir)/" && \
	$(mkinstalldirs) $(INSTALL_ROOT)$(sysconfdir) || :
	@$(INSTALL_DATA) sapi/fpm/jonj-fpm.conf $(INSTALL_ROOT)$(sysconfdir)/jonj-fpm.conf.default || :

	@echo "Installing JONJ FPM man page:      $(INSTALL_ROOT)$(mandir)/man8/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(mandir)/man8
	@$(INSTALL_DATA) sapi/fpm/jonj-fpm.8 $(INSTALL_ROOT)$(mandir)/man8/jonj-fpm$(program_suffix).8

	@echo "Installing JONJ FPM status page:   $(INSTALL_ROOT)$(datadir)/fpm/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(datadir)/fpm
	@$(INSTALL_DATA) sapi/fpm/status.html $(INSTALL_ROOT)$(datadir)/fpm/status.html
