<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller2.phar.tar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller2.phar.tar');
$a['a.jonj'] = 'hio';
$a['a.jpg'] = 'hio';
$a['a.jonjs'] = '<?jonj function hio(){}';
$a->setStub('<?jonj
Phar::webPhar("whatever", "a.jonj");
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
