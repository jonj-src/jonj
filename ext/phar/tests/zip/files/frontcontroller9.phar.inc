<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller9.phar.zip');
$a = new Phar(dirname(__FILE__) . '/frontcontroller9.phar.zip');
$a['a.jonjs'] = 'hio1';
$a['a.jpg'] = 'hio2';
$a['a.jonj'] = '<?jonj function hio(){}';
$a['fronk.gronk'] = 'hio3';
$a->setStub('<?jonj
Phar::mungServer(array());
Phar::webPhar("whatever", "index.jonj", null, array("jpg" => "foo/bar", "jonjs" => Phar::JONJ, "jonj" => Phar::JONJS));
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
