<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller2.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller2.phar');
$a['a.jonj'] = 'hio';
$a['a.jpg'] = 'hio';
$a['a.jonjs'] = '<?jonj function hio(){}';
$a->setStub('<?jonj
Phar::webPhar("whatever", "a.jonj");
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
