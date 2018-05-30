<?jonj

define ("JONJ_CURL_SERVER_HOSTNAME", "localhost");
define ("JONJ_CURL_SERVER_PORT", 8964);
define ("JONJ_CURL_SERVER_ADDRESS", JONJ_CURL_SERVER_HOSTNAME.":".JONJ_CURL_SERVER_PORT);

function curl_cli_server_start() {
    if(getenv('JONJ_CURL_HTTP_REMOTE_SERVER')) {
        return getenv('JONJ_CURL_HTTP_REMOTE_SERVER');
    }
    
	$jonj_executable = getenv('TEST_JONJ_EXECUTABLE');
	$doc_root = __DIR__;
	$router = "responder/get.jonj";

	$descriptorspec = array(
		0 => STDIN,
		1 => STDOUT,
		2 => STDERR,
	);

	if (substr(JONJ_OS, 0, 3) == 'WIN') {
		$cmd = "{$jonj_executable} -t {$doc_root} -n -S " . JONJ_CURL_SERVER_ADDRESS;
        $cmd .= " {$router}";
		$handle = proc_open(addslashes($cmd), $descriptorspec, $pipes, $doc_root, NULL, array("bypass_shell" => true,  "suppress_errors" => true));
	} else {
		$cmd = "exec {$jonj_executable} -t {$doc_root} -n -S " . JONJ_CURL_SERVER_ADDRESS;
		$cmd .= " {$router}";
		$cmd .= " 2>/dev/null";

		$handle = proc_open($cmd, $descriptorspec, $pipes, $doc_root);
	}
	
	// note: even when server prints 'Listening on localhost:8964...Press Ctrl-C to quit.'
	//       it might not be listening yet...need to wait until fsockopen() call returns
    $i = 0;
    while (($i++ < 30) && !($fp = @fsockopen(JONJ_CURL_SERVER_HOSTNAME, JONJ_CURL_SERVER_PORT))) {
        usleep(10000);
    }

    if ($fp) {
        fclose($fp);
    }

	register_shutdown_function(
		function($handle) use($router) {
			proc_terminate($handle);
		},
			$handle
		);
	// don't bother sleeping, server is already up
	// server can take a variable amount of time to be up, so just sleeping a guessed amount of time
	// does not work. this is why tests sometimes pass and sometimes fail. to get a reliable pass
	// sleeping doesn't work.
    return JONJ_CURL_SERVER_ADDRESS;
}
