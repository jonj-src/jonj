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
   |          Colin Viebrock <colin@viebrock.ca>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef INFO_H
#define INFO_H

#define JONJ_ENTRY_NAME_COLOR "#ccf"
#define JONJ_CONTENTS_COLOR "#ccc"
#define JONJ_HEADER_COLOR "#99c"

#define JONJ_INFO_GENERAL			(1<<0)
#define JONJ_INFO_CREDITS			(1<<1)
#define JONJ_INFO_CONFIGURATION		(1<<2)
#define JONJ_INFO_MODULES			(1<<3)
#define JONJ_INFO_ENVIRONMENT		(1<<4)
#define JONJ_INFO_VARIABLES			(1<<5)
#define JONJ_INFO_LICENSE			(1<<6)
#define JONJ_INFO_ALL				0xFFFFFFFF

#ifndef HAVE_CREDITS_DEFS
#define HAVE_CREDITS_DEFS

#define JONJ_CREDITS_GROUP			(1<<0)
#define JONJ_CREDITS_GENERAL			(1<<1)
#define JONJ_CREDITS_SAPI			(1<<2)
#define JONJ_CREDITS_MODULES			(1<<3)
#define JONJ_CREDITS_DOCS			(1<<4)
#define JONJ_CREDITS_FULLPAGE		(1<<5)
#define JONJ_CREDITS_QA				(1<<6)
#define JONJ_CREDITS_WEB             (1<<7)
#define JONJ_CREDITS_ALL				0xFFFFFFFF

#endif /* HAVE_CREDITS_DEFS */

#define JONJ_LOGO_DATA_URI "data:image/png;base64,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"
#define JONJ_EGG_LOGO_DATA_URI	"data:image/gif;base64,R0lGODlheQBAAOZ/AB0BAHB0r0xPkMmMdtjXu9CYh2sxAquv1pJsTmUCAKyQaaVnA6lwVYOGvJM1ANvbwYlPMJubzVYCAGgRAJhqLm9BCq99aLetkqpMAKJCAJdvYqtzHZZWAnNsmIUjAHdKNW8bALqST3ABAJpjEVYnA08xU5JLBG1FU8fKtIhcS4d0kYJFBLiQgkABAIQwAd3h0eDm2sXCpdXYxIiKwG05M7CtuH4UAVNGdNPWwJ5dAcB+ZOjv6NrdxqRMAN7ZuH1qeXZYYszQunp9toxRBJ2dz5yHaa6ANoVYHrWgebK54M28jdfbxpGTxpWWyY8tAJqFl6hHANzUr5J+UqOm0Z47AI9eG9LKolo1IEASAkNIjJSczVkUAYyPw1NNTqSdrYNgZ3hMGK9XAa5KAJNKGaGdv3k6BVgZE6BTGmVopNrezIJhNGgeFpJCKFogM9DTvpiQrm1afpiYxZxQClZZmKJSBaNGAWsnItLQscC8p7NaEI47BpdhQGwVFy0bH18LAJmZzCH5BAEAAH8ALAAAAAB5AEAAAAf/gH+Cg4SFhoeIiYIRRERTj48HkgeQU40Rf3FNm5yLEZ+YhnFeTYqmp6ipioyOU5OSSbGys0mTkERaTUxcM729XExMpaCYTTU+NaWqy8yoo3Gtk7SxlGRvT08qPz9f3UBAX0A/KuQNXOfnMw0NQu3t5n+YNQ9RXs33+IOfeFE1B7MHyGTjdoLGGjN+JChcyLChHz981tixQ+MEEDhwOgRoFyAAmo1C3rh5gEdZvpOnHB2Y56ZGkilvfpyws6WFzRYMH+rcOaGnz58+/UwQ2lPiiYxokqLpgIKHlSeXUEolpPLfgRgv8KQw6CdBAoU3cUp4OHanWbNA06aleOIGGjw8/x4gQSOkCZFQU+9FcPWv1hsvBF4g6fpwKNiwDRMvPMu4sdk1F+IqgZNFABoujPDmTemK2pOZWyKnKVK4Z8KwYhXnHLvYsWudSNLQ+5KltoA5Qv7c3axIZSwyKuwstBMkLgKipv2gVpjw9Vm1yNWajv3gAYUtbW7UzoLbLm9DRGAleUIjYQKyCNLwwKEG6OmbzBlLTwtiPlAQ+CdQ5xGiPog1JWwngBCMfKdbZweoUF5ZO8XGwx0QIEcWYqVF9xMILtyHX349bejhh/ghERcPSHjgIYDacdfAJ5t1Np5w551lRwxpLGEFGNG9J5aFaq2wAYge+gQkkBfI9oASZXw4VP+AtaHhHUrhxfIGDa2ZJcEHbvBgI44/6cicWkFWYcSQHE5ApodlxLBEdVasoGRXZjA54G74IPiFYlYisISWN+YoAXxjgbnhBBT0ZyaQQx16JghgBFHdA3dU8eZXEmTX5JPMdPbEGsw15NWnEmjAQ1wQJpCcjjgVxhNQRoRggIb1NafokBMggMMDa95BwZtfgsDkHDMQscxektzZqQSfeoWsskUYecceySrbAgA2MRhtshO0WoYEIng1galDyXpmTwrI8AAPBBBAQYemLXRoAm3Y1oCwqBBLBg2pqdYQC6M+QAAD0f5J4bXRghBCCCssS3CgIPbUrVcX4IruvwG7G6v/QiUIkIUQ9JriSBxt5JvQQoAudMGe/iqQ2HJdUaqYAQeboNBXyi503qz1PfzVGjFInC4CoDaUn8tMzmvKXm/wIYIIEzDdU9AuM3dyXAQM1tByyJJc7UJlGLEBByNrXa1QSlJqEwIEnLsEAVEgIHa1Zo58U8Ybd3wIEW+wYYMHfPftgQ2ABy444HigTIASeizddIw6PV1z1BKUscACOYBwNXxEefjVTWZckLaWBDx7GLXUtlDf6DYFKAAXdg8ShxZxsOGEA7TX7sTtuOeOux6F93uHER444ffwfA8uOAhl5LBB5QnYIILzDzO9oYlDK5uCGwTsuQQOpb5NFKo2aTdH/xOa/bGJFinMXvv66jvQvu28S+wvEnrYjrv77Ot++wqFhrCBCSAI3u36pjsXuGCAfkNC2s4lAxzEYAzG6xDfQGCDpz0kAXzQDhpaxwQtaOB96wuhCGmnh5OtyV9WoID6bjfCEbqgAmBQgxQUoIYKkAALIGwh+xhwh8+thwB4GIP+cHdA3fmNBrUxmiCCMYO/8c2AUDQg3+7nAD1Y8YoXkIHhohAC/OVOh7SjggGwcAUjrIAEN7yhC6gARhGeQQnpekCNcIAD+rURjOIjn/m4oIUTLG1pftjCFu4zqydKEQky0GJ1CJDCvf3NkQSkogNWYAADgEEKBiiDCVZgAgNgqP+NbGSjHhT4uTm6QQE5vGPtylAbjkVgFypQWAKmBYDSoQZrCtFAIlHmgygowQM6+9QfB5dJTnLABENYAAdykIMVlMGTASTeFIXnBAX0sDppSIMM3BAECghQdyS84gipQIUTZGGDfwDGCa5Wy1rS0p3tbOdNUoADLcall1ycwJ/2abMELC0BWzCACUzAzMkpcwQcoMMxDbCFlkXrYR6wQA8/94LtuSEGegPch55oABd0NIDRdEEVkbi6JsyAC21gpzzjyVKWXgEFu6wOPkNwy5IF1Jg5MKgyc8CBZXLgjDWpaQs+cIEoxPEBL3hBA4OABH1m7ZZYiKpUWyBVQYKABNr/CQAvhOBQf4pAJ6wRGE5ustIWZNGe94xCFJCAr5pKIKBlOMIQlpnTyXFgBENg5k9dQIKatNMMNEAAEqxg1AWmAQZL2GYQ9gCod7b0se0MEBp6oQLBCXB4FOxJBSvYpYQgwA27vCc+rRCCD5hBrG8lQRlWMIS5Us6gd6XrT58pyCsUAQlKIGwvCeCD6vAABtrEgRsusIZbQva4tezDbRowgy+Q87nQfa77vjhCJ4whBvVcAsoe4IPu9tIKSkACEsZw1UyWYa50hS1e9brJZ5YBCdxVa3cXeC4YKFW4QUBATZF7XOUKYB3OzYCAB0xgAUe3wASmAhLckN1+ydS7vVXC/wIqUIFiznUIIzBCMimHUMod05mrDQF3e/soXB0Wsfi9QBlq9jaBwROyfagMgKmA4BrbuMBUoAAKGIzWuDy4u0agwAoqvNqecqAKajDA5HiazGNucgWsNcKtSoyrF+xgB4nFQRBQsAf9+S2a1PMQ4CZQwYfE678NCPCN12zjBdezxz5GqhKMgAAwEBkMHEjmCI4wgg0sYAQddjKUh3CEDVghLtrdEwyuHFxu0i/BNM5AdKVbO3LSDnckXccPqFCHTnu6DgMGNZsLfIYY8BjOcYEBEqRAATBU0gAryPMCNkBrP+N1BMoc6KCrQIEi7SkNL1g0lhvIzQuMIdI2FnUGQP/d6Rt/gDszEEIH9NCDalv72tX+tLa3DWoG7PjN2h0VD9QjBSkcwYYGqEAVWjvrWucA0Lh28mqHUIUqKGAJSRU2YokdBDzswQFrbnaoPW1jc86BCe2ggR4GynCGy8EEcpADtieO7TooIAinTvQSYBCDctuQBOmWKwfaTWtAVyGnFAYxvdWAABRc+cr7xq+/HaDsAn9awNyucVb/IIQGlEBRJuooFMtQBhdcUQ9QbnjDHy4HJGCcx4lWtRTUcIU+AMCS6+ZArUsO6MlVGOQkgKEMLwDzRgeh3/+GArc9rfZtt13tOK8DFNhQmxlEINpw8OofFee0n1Rwo5W8KsgreXH/jGd3CaMpQpLRmO5193kDXtvACCjA668N4eNhl2ERkppI/AbhAhAAOIHrQPHSX5vtUDD4JxAuhDbofe+wj33sN1c6P7TcDbiXQRpQUO4roLECZahAax9vBK8hNAdVQOgQKPn7GEoBBdrk5pYVYIdDmcgDRh9oxOVAh+7TYfveD3/45aAdjgmiHR1owetlz/7Zw6cFKbjA2d2QBjyUGwxXgOERKECBIVCA1sW3AYVCAUbQZ3jlTBQGBmBAVGlwdp+HAEFFISBQSURXBkd3gWNgAhmYgQxnTgLABJjwSu1QAgDALe13grH3fmZQBDEQBDJwAWpQQxUGQzBUBZLHa17D/2uUdwRHAAaU9GowpIB4cFFFYAaP9X5YECiD8zyD4zc2YAcaY36L0AAdYXUmmCwoyH4lcyVFgAJFgEaMl3IrMAL1FgL8t3WS11o/CIZYgEZqUAQf4Fjy9H7IkoWyJz6agQntgAbU4k8EQzB/tH4SQC0MgRMfAAFt2IbpBgaERnlqQEMh0CqRRwE5sHzPBIYkcAWl8ydHmC92KHt0wzqFEAFN0A5zgBN/mIrC9EeDaEtjFTljBHL6t39HQGeCFV5IEIn8VwVQdolYAAZGYAC21IoshRjr94nxkgVKNIpcsBFwsCzBpIoPNUvFeBMhcASKGENqoIBgcARqQAEIgAD8h/8AVcCDdvZ1NwQGFIAF7mQTR0g66vdPWPh6WNgGGhMArUMIEdCMQgAHLuCExrNZ7ceJLTVUaOR7QdiNPMhy4RiOu1iOllhJNxRVAIAFQdWJ7uRVgBiNCcAkAcAiiXB3G6ECxyZpk8ZG04VAxIMfVlUfE7AFUZV5e7AHDMAAFnCTLJCTOWkBNbkHEPCTdiaR7EhVQ+mO03JLsiSNXkE3aACSq9CMGwEBUDCVVAkFGHCVVZmVU3lzBQYCZJQCNqkDOjAAZEmWBXCWaHmWAyCWOsCTDIAAKQAGE7kYp0IWwqSUXuEHKYKP5ROSCNcRKQAFYmCVV1mYhjmVhpmYislXYGn/AWwplmWZlgVQlo8pljX5llcQVSAgSbkDkIHzPLJnKXPCDKQoBCMplYqZmqqpmCZwBY3pmGUZmWpplmSpAwzAlpe5B5qIBVQgBoO5lYSZlaJ2kuSkB+YEbflYLxFAhR3xBWcgBqsZnalpAGAZjrc5AJKZnWt5mbfJkzO5m78JnNKplWJQBx+QIk6SnKpQmh0RACmAAdApnfK5AofIcimQAjSJnZJpmxqwBw2JADTplhBgdSQQnlQpn4Q5mBCQIv9FJyeBCdHWER2QAnIQn/KZmnnwAR9wnxrQoRoKlpFpARqQAh9AAynQoXAJljWpiR/AABmglYIZnb6JAQtqGwGA/ylTgQnMCZjPaaEXepUZ+gEacJMpEA5AwKFrWQA8eZ9F2qE3qQE2iYgAcJts8KJZuZq+CQU12iRM4KC8QYo7GgA/sAcx+qMYEAZlkAIsYAE0IA5X0AVF0KFoKaL3+QNv2gU0wAAswAA0AAEf0Ad7oANsQAVZ6aNX6ZtiMAZtYRuX4aUG8glNEKYTegY9YKiqGQYGIKQIoKFf0AU/0KEpYAGTeZ/hqAKe+gUfgAAaYKIWgIhUSqhVGZ+IigFjsKWV0ah9aSCekE6m2Z4d8AUQQAfwGZ1jQKIMsAcc2qEasKEDwABCKqKqOqIpgACOaZt+OpmDGp6I6puKmiK10R2Oqv+rh7B67NCeAfCrM3kGYbCu7LquNokANnmT8moBV8AAa+ma88qTN1kAlgkBGjCZepAB21oHtbqo2zEHaLAi4SquIUmuHfERH9EB3CCOW2cEPXmbY0mWzmqvOnCiZKmTFoAAYymWezAGgcoAhFoHbPABBnuwAWB3jcCw+MAiTBChSqEUErsNnzqvbbmWPmmZNJmxssmvx/oB4AAEbeGt30oXDaAb6imzpEmz5XqzVNsBHUAOKoAN3XCZRWoB2PC124ARAqAx2yEglvGydhEVUPsdoPAHuxChHoEGczC3dFu3c3sbY0u2Zbu3t/ERQnBSaeuUayuzoLAbTcALU6sUdnsSt2Nrt0mxETPABE57Cbmqq4EAADs="
#define ZEND_LOGO_DATA_URI "data:image/png;base64,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"

BEGIN_EXTERN_C()
JONJ_FUNCTION(jonjversion);
JONJ_FUNCTION(jonjinfo);
JONJ_FUNCTION(jonjcredits);
JONJ_FUNCTION(jonj_sapi_name);
JONJ_FUNCTION(jonj_uname);
JONJ_FUNCTION(jonj_ini_scanned_files);
JONJ_FUNCTION(jonj_ini_loaded_file);
JONJAPI char *jonj_info_html_esc(char *string TSRMLS_DC);
JONJAPI void jonj_info_html_esc_write(char *string, int str_len TSRMLS_DC);
JONJAPI void jonj_print_info_htmlhead(TSRMLS_D);
JONJAPI void jonj_print_info(int flag TSRMLS_DC);
JONJAPI void jonj_print_style(void);
JONJAPI void jonj_info_print_style(TSRMLS_D);
JONJAPI void jonj_info_print_table_colspan_header(int num_cols, char *header);
JONJAPI void jonj_info_print_table_header(int num_cols, ...);
JONJAPI void jonj_info_print_table_row(int num_cols, ...);
JONJAPI void jonj_info_print_table_row_ex(int num_cols, const char *, ...);
JONJAPI void jonj_info_print_table_start(void);
JONJAPI void jonj_info_print_table_end(void);
JONJAPI void jonj_info_print_box_start(int bg);
JONJAPI void jonj_info_print_box_end(void);
JONJAPI void jonj_info_print_hr(void);
JONJAPI void jonj_info_print_module(zend_module_entry *module TSRMLS_DC);
JONJAPI char *jonj_get_uname(char mode);

void register_jonjinfo_constants(INIT_FUNC_ARGS);
END_EXTERN_C()

#endif /* INFO_H */

