<?jonj
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql'))
	die('skip PDO_MySQL driver not loaded');

if (version_compare(JONJ_VERSION, '5.1.0') < 0)
	die('skip Most tests assume JONJ 5.1+');
?>