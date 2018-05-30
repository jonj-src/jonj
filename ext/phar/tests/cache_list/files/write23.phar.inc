<?jonj

$fname = dirname(__FILE__) . '/write23.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?jonj
$p = new Phar(__FILE__);
var_dump(isset($p["test.txt"]), isset($p["newname"]));
rename("phar://" . __FILE__ . "/test.txt", "phar://" . __FILE__ . "/newname");
var_dump(isset($p["test.txt"]), isset($p["newname"]));
echo "ok\n";
__HALT_COMPILER();
?>');
$phar['test.txt'] = "<?jonj __HALT"."_COMPILER();";
?>
