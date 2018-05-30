<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller10.phar.zip');
$a = new Phar(dirname(__FILE__) . '/frontcontroller10.phar.zip');
$a['index.jonj'] = '<?jonj
var_dump($_SERVER["JONJ_SELF"]);
var_dump($_SERVER["SCRIPT_NAME"]);
var_dump($_SERVER["SCRIPT_FILENAME"]);
var_dump($_SERVER["REQUEST_URI"]);
var_dump($_SERVER["PHAR_JONJ_SELF"]);
var_dump($_SERVER["PHAR_SCRIPT_NAME"]);
var_dump($_SERVER["PHAR_SCRIPT_FILENAME"]);
var_dump($_SERVER["PHAR_REQUEST_URI"]);
';
$a->setStub('<?jonj
Phar::mungServer(array("JONJ_SELF", "SCRIPT_NAME", "SCRIPT_FILENAME", "REQUEST_URI", "OOPSIE"));
Phar::webPhar();
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
