<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller16.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller16.phar');
$a['index.jonj'] = '<?jonj
echo "hi";
';
$a->setStub('<?jonj
try {
Phar::webPhar("test.phar", "/index.jonj", null, array(), array("fail", "here"));
} catch (Exception $e) {
die($e->getMessage() . "\n");
}
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
