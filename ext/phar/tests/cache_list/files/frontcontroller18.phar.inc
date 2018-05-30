<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller18.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller18.phar');
$a['index.jonj'] = '<?jonj
echo "hi";
';
$a->setStub('<?jonj
function s($a)
{
}
try {
Phar::webPhar("test.phar", "/index.jonj", null, array(), "s");
} catch (Exception $e) {
die($e->getMessage() . "\n");
}
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
