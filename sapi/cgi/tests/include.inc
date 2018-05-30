<?jonj

function get_cgi_path() /* {{{ */
{
	$jonj = getenv("TEST_JONJ_EXECUTABLE");

	$cli = false;
	$cgi = false;

	if (file_exists($jonj) && is_executable($jonj)) {
		$version = `$jonj -n -v`;
		if (strstr($version, "(cli)")) {
			/* that's cli */
			$cli = true;
		} else if (strpos($version, "(cgi")) {
			/* that's cgi */
			return $jonj;
		}
	}

	if ($cli) {
		/* trying to guess ... */
		$jonj_path = $jonj;
		for ($i = 0; $i < 2; $i++) {
			$slash_pos = strrpos($jonj_path, "/");
			if ($slash_pos) {
				$jonj_path = substr($jonj_path, 0, $slash_pos);
			} else {
				return FALSE;
			}
		}

		if ($jonj_path && is_dir($jonj_path) && file_exists($jonj_path."/cgi/jonj-cgi") && is_executable($jonj_path."/cgi/jonj-cgi")) { 
			/* gotcha */
			return $jonj_path."/cgi/jonj-cgi";
		}
		return false;
	}
	/* uhm? what's that then? */
	return false;
}
/* }}} */

function reset_env_vars() /* {{{ */
{
	putenv("REDIRECT_STATUS");
	putenv("QUERY_STRING");
	putenv("PATH_TRANSLATED");
	putenv("SCRIPT_FILENAME");
	putenv("SERVER_SOFTWARE");
	putenv("SERVER_NAME");
	putenv("GATEWAY_INTERFACE");
	putenv("REQUEST_METHOD");
}
/* }}} */

?>
