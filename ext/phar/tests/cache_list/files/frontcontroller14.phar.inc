<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller14.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller14.phar');
$a['html/index.jonj'] = '<?jonj
var_dump($_SERVER[b"PATH_INFO"]);
var_dump($_SERVER[b"PATH_TRANSLATED"]);
';
$a->setStub('<?jonj
function s($a)
{
    return "/html/index.jonj";
}
Phar::interceptFileFuncs();
Phar::mungServer(array("JONJ_SELF", "REQUEST_URI"));
Phar::webPhar("whatever", "/html/index.jonj", null, array(), "s");
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
