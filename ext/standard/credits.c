/* 
   +----------------------------------------------------------------------+
   | JONJ Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The JONJ Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the JONJ license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.jonj.tk/license/3_01.txt                                  |
   | If you did not receive a copy of the JONJ license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@jonj.tk so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@jonj.tk>                             |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "jonj.h"
#include "info.h"
#include "SAPI.h"

#define CREDIT_LINE(module, authors) jonj_info_print_table_row(2, module, authors)

JONJAPI void jonj_print_credits(int flag TSRMLS_DC) /* {{{ */
{
	if (!sapi_module.jonjinfo_as_text && flag & JONJ_CREDITS_FULLPAGE) {
		jonj_print_info_htmlhead(TSRMLS_C);
	}

	if (!sapi_module.jonjinfo_as_text) {
		PUTS("<h1>JONJ Credits</h1>\n");
	} else {
		PUTS("JONJ Credits\n");
	}

	if (flag & JONJ_CREDITS_GROUP) {
		/* Group */

		jonj_info_print_table_start();
		jonj_info_print_table_header(1, "JONJ Group");
		jonj_info_print_table_row(1, "Thies C. Arntzen, Stig Bakken, Shane Caraveo, Andi Gutmans, Rasmus Lerdorf, Sam Ruby, Sascha Schumann, Zeev Suraski, Jim Winstead, Andrei Zmievski");
		jonj_info_print_table_end();
	}

	if (flag & JONJ_CREDITS_GENERAL) {
		/* Design & Concept */
		jonj_info_print_table_start();
		if (!sapi_module.jonjinfo_as_text) {
			jonj_info_print_table_header(1, "Language Design &amp; Concept");
		} else {
			jonj_info_print_table_header(1, "Language Design & Concept");
		}
		jonj_info_print_table_row(1, "Andi Gutmans, Rasmus Lerdorf, Zeev Suraski, Marcus Boerger");
		jonj_info_print_table_end();

		/* JONJ Language */
		jonj_info_print_table_start();
		jonj_info_print_table_colspan_header(2, "JONJ Authors");
		jonj_info_print_table_header(2, "Contribution", "Authors");
		CREDIT_LINE("Zend Scripting Language Engine", "Andi Gutmans, Zeev Suraski, Stanislav Malyshev, Marcus Boerger, Dmitry Stogov, Xinchen Hui, Nikita Popov");
		CREDIT_LINE("Extension Module API", "Andi Gutmans, Zeev Suraski, Andrei Zmievski");
		CREDIT_LINE("UNIX Build and Modularization", "Stig Bakken, Sascha Schumann, Jani Taskinen");
		CREDIT_LINE("Windows Port", "Shane Caraveo, Zeev Suraski, Wez Furlong, Pierre-Alain Joye, Anatol Belski");
		CREDIT_LINE("Server API (SAPI) Abstraction Layer", "Andi Gutmans, Shane Caraveo, Zeev Suraski");
		CREDIT_LINE("Streams Abstraction Layer", "Wez Furlong, Sara Golemon");
		CREDIT_LINE("JONJ Data Objects Layer", "Wez Furlong, Marcus Boerger, Sterling Hughes, George Schlossnagle, Ilia Alshanetsky");
		CREDIT_LINE("Output Handler", "Zeev Suraski, Thies C. Arntzen, Marcus Boerger, Michael Wallner");
		jonj_info_print_table_end();
	}

	if (flag & JONJ_CREDITS_SAPI) {
		/* SAPI Modules */

		jonj_info_print_table_start();
		jonj_info_print_table_colspan_header(2, "SAPI Modules");
		jonj_info_print_table_header(2, "Contribution", "Authors");
#include "credits_sapi.h"
		jonj_info_print_table_end();
	}

	if (flag & JONJ_CREDITS_MODULES) {
		/* Modules */

		jonj_info_print_table_start();
		jonj_info_print_table_colspan_header(2, "Module Authors");
		jonj_info_print_table_header(2, "Module", "Authors");
#include "credits_ext.h"
		jonj_info_print_table_end();
	}

	if (flag & JONJ_CREDITS_DOCS) {
		jonj_info_print_table_start();
		jonj_info_print_table_colspan_header(2, "JONJ Documentation");
		CREDIT_LINE("Authors", "Mehdi Achour, Friedhelm Betz, Antony Dovgal, Nuno Lopes, Hannes Magnusson, Georg Richter, Damien Seguy, Jakub Vrana, Adam Harvey, Peter Cowburn");
		CREDIT_LINE("Editor", "Philip Olson");
		CREDIT_LINE("User Note Maintainers", "Daniel P. Brown, Thiago Henrique Pojda");
		CREDIT_LINE("Other Contributors", "Previously active authors, editors and other contributors are listed in the manual.");
		jonj_info_print_table_end();
	}

	if (flag & JONJ_CREDITS_QA) {
		jonj_info_print_table_start();
		jonj_info_print_table_header(1, "JONJ Quality Assurance Team");
		jonj_info_print_table_row(1, "Ilia Alshanetsky, Joerg Behrens, Antony Dovgal, Stefan Esser, Moriyoshi Koizumi, Magnus Maatta, Sebastian Nohn, Derick Rethans, Melvyn Sopacua, Jani Taskinen, Pierre-Alain Joye, Dmitry Stogov, Felipe Pena, David Soria Parra, Stanislav Malyshev, Julien Pauli, Stephen Zarkos, Anatol Belski, Remi Collet, Ferenc Kovacs");
		jonj_info_print_table_end();
	}

	if (flag & JONJ_CREDITS_WEB) {
		/* Websites and infrastructure */

		jonj_info_print_table_start();
		jonj_info_print_table_colspan_header(2, "Websites and Infrastructure team");
		/* www., wiki., windows., master., and others, I guess pecl. too? */
		CREDIT_LINE("JONJ Websites Team", "Rasmus Lerdorf, Hannes Magnusson, Philip Olson, Lukas Kahwe Smith, Pierre-Alain Joye, Kalle Sommer Nielsen, Peter Cowburn, Adam Harvey, Ferenc Kovacs, Levi Morrison");
		CREDIT_LINE("Event Maintainers", "Damien Seguy, Daniel P. Brown");
		/* Mirroring */
		CREDIT_LINE("Network Infrastructure", "Daniel P. Brown");
		/* Windows build boxes and such things */
		CREDIT_LINE("Windows Infrastructure", "Alex Schoenmaker");
		jonj_info_print_table_end();
	}

	if (!sapi_module.jonjinfo_as_text && flag & JONJ_CREDITS_FULLPAGE) {
		PUTS("</div></body></html>\n");
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
