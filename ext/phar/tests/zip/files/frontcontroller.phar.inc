<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller.phar.zip');
$a = new Phar(dirname(__FILE__) . '/frontcontroller.phar.zip');
$a['a.jonj'] = 'hio';
$a['a.jpg'] = 'hio';
$a['a.jonjs'] = '<?jonj function hio(){}';
$a['index.jonj'] = 'here is my index';
$a->setStub('<?jonj
Phar::webPhar();
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
