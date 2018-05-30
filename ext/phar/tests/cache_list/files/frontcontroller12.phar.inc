<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller12.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller12.phar');
$a['index.jonj'] = '<?jonj
var_dump($_SERVER["JONJ_SELF"]);
var_dump($_SERVER[b"SCRIPT_NAME"]);
var_dump($_SERVER[b"SCRIPT_FILENAME"]);
var_dump($_SERVER[b"REQUEST_URI"]);
var_dump($_SERVER[b"PHAR_JONJ_SELF"]);
var_dump($_SERVER[b"PHAR_SCRIPT_NAME"]);
var_dump($_SERVER[b"PHAR_SCRIPT_FILENAME"]);
var_dump($_SERVER[b"PHAR_REQUEST_URI"]);
';
$a->setStub('<?jonj
Phar::mungServer(array("JONJ_SELF", "SCRIPT_NAME", "SCRIPT_FILENAME", "REQUEST_URI"));
Phar::webPhar();
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
