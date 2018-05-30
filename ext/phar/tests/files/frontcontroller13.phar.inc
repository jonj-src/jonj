<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller13.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller13.phar');
$a['index.jonj'] = '<?jonj
var_dump("test");
include "oof/test.jonj";';
$a['oof/test.jonj'] = '<?jonj
var_dump("oof/test.jonj");
include "./hi.jonj";';
$a['oof/hi.jonj'] = '<?jonj
var_dump("hi");';
$a->setStub('<?jonj
Phar::webPhar();
__HALT_COMPILER();');