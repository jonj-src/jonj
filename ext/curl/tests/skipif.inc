<?jonj
    if (!extension_loaded("curl")) exit("skip curl extension not loaded");
    if(false === getenv('JONJ_CURL_HTTP_REMOTE_SERVER')) {
        if (jonj_sapi_name() != "cli") {
                die("skip JONJ_CURL_HTTP_REMOTE_SERVER env variable is not defined");
        }
    }
