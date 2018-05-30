<?jonj

$fname = dirname(__FILE__) . '/write10.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?jonj
$p = new Phar(__FILE__);
var_dump($p->getStub());
$p2 = new Phar(__FILE__);
$a = fopen("phar://" . __FILE__ . "/test.txt", "r");
$p->setStub($a);
echo $p2->getStub(),"\n";
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?jonj __HALT"."_COMPILER();";
?>
