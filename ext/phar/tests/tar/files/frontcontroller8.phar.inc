<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller8.phar.tar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller8.phar.tar');
$a['a.jonjs'] = 'hio1';
$a['a.jpg'] = 'hio2';
$a['a.jonj'] = '<?jonj function hio(){}';
$a['fronk.gronk'] = 'hio3';
$a->setStub('<?jonj
Phar::webPhar("whatever", "index.jonj", null, array("jpg" => "foo/bar", "jonjs" => Phar::JONJ, "jonj" => Phar::JONJS));
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
