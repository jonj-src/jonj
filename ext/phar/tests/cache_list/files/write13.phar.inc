<?jonj

$fname = dirname(__FILE__) . '/write13.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?jonj
$p = new Phar(__FILE__);
var_dump($p["test.txt"]->isCompressed());
$p2 = new Phar(__FILE__);
$p->compressFiles(Phar::GZ);
var_dump($p["test.txt"]->isCompressed());
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?jonj __HALT"."_COMPILER();";
?>
