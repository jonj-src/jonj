<?jonj

$fname = dirname(__FILE__) . '/write24.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?jonj
$p = new Phar(__FILE__);
var_dump(isset($p["newname"]));
$fp = fopen("phar://" . __FILE__ . "/newname", "w");
fwrite($fp, b"hi");
fclose($fp);
var_dump(isset($p["newname"]));
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?jonj __HALT"."_COMPILER();";
?>