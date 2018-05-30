<?jonj

$fname = dirname(__FILE__) . '/write18.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?jonj
$p = new Phar(__FILE__);
echo decoct(fileperms("phar://" . __FILE__ . "/test.txt")),"\n";
$p["test.txt"]->chmod(0444);
echo decoct(fileperms("phar://" . __FILE__ . "/test.txt")),"\n";
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?jonj __HALT"."_COMPILER();";
$phar["test.txt"]->chmod(0666);
?>
