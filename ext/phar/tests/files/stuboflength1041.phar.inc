<?jonj

@unlink(__DIR__ . '/stuboflength1041.phar');

$phar = new Phar('./stuboflength1041.phar');
$phar['a.jonj'] = 'hi1';
$phar['b.jonj'] = 'hi2';

$phar->setStub('<?jonj
/***stub of length 1041 including the halt compiler*********************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
*********************************************/
__HALT_COMPILER();');