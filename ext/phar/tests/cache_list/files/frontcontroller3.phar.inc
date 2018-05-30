<?jonj
@unlink(dirname(__FILE__) . '/frontcontroller3.phar');
$a = new Phar(dirname(__FILE__) . '/frontcontroller3.phar');
$a['a.jonj'] = 'hio';
$a['a.jpg'] = 'hio';
$a['a.jonjs'] = '<?jonj function hio(){}';
$a->setStub('<?jonj
function s($a)
{
    static $b = array(b"/hi" => "a.jonjs");
    if (isset($b[$a])) return $b[$a];
    return $a;
}
Phar::webPhar("whatever", "/index.jonj", null, array(), "s");
echo "oops did not run\n";
var_dump($_ENV, $_SERVER);
__HALT_COMPILER();');
?>
