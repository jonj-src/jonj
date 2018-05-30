<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller6.phar.zip');
$a = new Phar(dirname(__FILE__) . '/frontcontroller6.phar.zip');
$a['a.jonj'] = 'hio';
$a['a.jpg'] = 'hio';
$a['a.jonjs'] = '<?jonj function hio(){}';
$a->setStub('<?jonj
Phar::webPhar("whatever", "index.jonj", null, array("jonj" => 100));
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
