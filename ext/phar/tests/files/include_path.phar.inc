<?jonj
// test advanced example
$phar = new Phar(dirname(__FILE__) . '/include_path.phar');
$phar2 = new Phar(dirname(__FILE__) . '/include_path2.phar');
$phar2['file1.jonj'] = 'file1.jonj
';
$phar2['test/file1.jonj'] = 'test/file1.jonj
';
$phar['hello/test.jonj'] = '<?jonj
include "file1.jonj";';

$phar->setStub("<?jonj
set_include_path('.' . PATH_SEPARATOR . 'phar://' . dirname(__FILE__) . '/files/include_path2.phar' );
include 'phar://' . __FILE__ . '/hello/test.jonj';
set_include_path('.' . PATH_SEPARATOR . 'phar://' . dirname(__FILE__) . '/files/include_path2.phar/test');
include 'phar://' . __FILE__ . '/hello/test.jonj';
echo \"ok\\n\";
__HALT_COMPILER();
?>");
