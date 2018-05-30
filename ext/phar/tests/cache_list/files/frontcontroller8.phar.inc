<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller8.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller8.phar');
$a['a.jonjs'] = 'hio1';
$a['a1.jonjs'] = '<?jonj var_dump($_SERVER[b"REQUEST_URI"], $_SERVER[b"PATH_INFO"]);';
$a['a.jpg'] = 'hio2';
$a['a.jonj'] = '<?jonj function hio(){}';
$a['fronk.gronk'] = 'hio3';
$a['404.jonj'] = 'My 404 is rawesome';
$a['noext'] = 'hi';
$a['unknown.ext'] = '<?jonj var_dump("hi");';
$a['bigfile.txt'] = str_repeat('a', 8193);
$a['fatalerror.jonjs'] = '<?jonj oopsie_daisy();';
$a->setStub('<?jonj
Phar::webPhar("whatever", "index.jonj", "404.jonj", array("jpg" => "foo/bar", "jonjs" => Phar::JONJ, "jonj" => Phar::JONJS));
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
