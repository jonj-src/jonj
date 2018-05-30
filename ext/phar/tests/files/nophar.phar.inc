<?jonj
$fname = dirname(__FILE__) . '/nophar.phar';
@unlink($fname);
$p = new Phar($fname);
$p['index.jonj'] = '<?jonj include "b/c.jonj";' . "\n";
$p['web.jonj'] = '<?jonj echo "web\n";';
$p['b/c.jonj'] = '<?jonj echo "in b\n";$a = fopen("index.jonj", "r", true);echo stream_get_contents($a);fclose($a);include dirname(__FILE__) . "/../d";';
$p['d'] = "in d\n";
$p->setStub($p->createDefaultStub('index.jonj', 'web.jonj'));
?>