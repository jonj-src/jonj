<?jonj

$fname = dirname(__FILE__) . '/write9.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?jonj
$p = new Phar(__FILE__);
var_dump($p->getStub());
$p2 = new Phar(__FILE__);
$p->setStub("<?jonj __HALT"."_COMPILER();");
echo $p2->getStub(),"\n";
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "hi
";
?>
