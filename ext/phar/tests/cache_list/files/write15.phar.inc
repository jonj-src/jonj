<?jonj

$fname = dirname(__FILE__) . '/write15.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?jonj
$p = new Phar(__FILE__);
var_dump(isset($p["copied"]));
$p->copy("test.txt","copied");
var_dump(isset($p["copied"]));
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?jonj __HALT"."_COMPILER();";
?>
