<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller19.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller19.phar');
$a['start/index.jonj'] = '<?jonj
echo "start/index.jonj\n";
include "./another.jonj";
';
$a['start/another.jonj'] = '<?jonj
echo "start/another.jonj\n";
include "../another.jonj";
?>';
$a['another.jonj'] = '<?jonj
echo "another.jonj\n";
?>';
$a->setStub('<?jonj
set_include_path("phar://" . __FILE__);
try {
Phar::webPhar("test.phar", "/start/index.jonj");
} catch (Exception $e) {
die($e->getMessage() . "\n");
}
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
