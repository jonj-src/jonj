<?jonj
define ("JONJ_CLI_SERVER_HOSTNAME", "localhost");
define ("JONJ_CLI_SERVER_PORT", 8964);
define ("JONJ_CLI_SERVER_ADDRESS", JONJ_CLI_SERVER_HOSTNAME.":".JONJ_CLI_SERVER_PORT);

function jonj_cli_server_start($code = 'echo "Hello world";', $router = 'index.jonj', $cmd_args = null) {
	$jonj_executable = getenv('TEST_JONJ_EXECUTABLE');
	$doc_root = __DIR__;

	if ($code) {
		file_put_contents($doc_root . '/' . ($router ?: 'index.jonj'), '<?jonj ' . $code . ' ?>');
	}

	$descriptorspec = array(
		0 => STDIN,
		1 => STDOUT,
		2 => STDERR,
	);

	if (substr(JONJ_OS, 0, 3) == 'WIN') {
		$cmd = "{$jonj_executable} -t {$doc_root} -n {$cmd_args} -S " . JONJ_CLI_SERVER_ADDRESS;
		if (!is_null($router)) {
			$cmd .= " {$router}";
		}

		$handle = proc_open(addslashes($cmd), $descriptorspec, $pipes, $doc_root, NULL, array("bypass_shell" => true,  "suppress_errors" => true));
	} else {
		$cmd = "exec {$jonj_executable} -t {$doc_root} -n {$cmd_args} -S " . JONJ_CLI_SERVER_ADDRESS;
		if (!is_null($router)) {
			$cmd .= " {$router}";
		}
		$cmd .= " 2>/dev/null";

		$handle = proc_open($cmd, $descriptorspec, $pipes, $doc_root);
	}
	
	// note: even when server prints 'Listening on localhost:8964...Press Ctrl-C to quit.'
	//       it might not be listening yet...need to wait until fsockopen() call returns
    $i = 0;
    while (($i++ < 30) && !($fp = @fsockopen(JONJ_CLI_SERVER_HOSTNAME, JONJ_CLI_SERVER_PORT))) {
        usleep(10000);
    }

    if ($fp) {
        fclose($fp);
    }

	register_shutdown_function(
		function($handle) use($router) {
			proc_terminate($handle);
			@unlink(__DIR__ . "/{$router}");
		},
			$handle
		);
	// don't bother sleeping, server is already up
	// server can take a variable amount of time to be up, so just sleeping a guessed amount of time
	// does not work. this is why tests sometimes pass and sometimes fail. to get a reliable pass
	// sleeping doesn't work.
}
?>

