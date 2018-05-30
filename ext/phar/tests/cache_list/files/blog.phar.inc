<?jonj

$fname = dirname(__FILE__) . '/blog.phar';
@unlink($fname);

$phar = new Phar($fname);
$phar->setStub('<?jonj if(file_exists(dirname(__FILE__) . "/files/config.xml")) {
    Phar::mount("config.xml", dirname(__FILE__) . "/files/config.xml");
}
Phar::webPhar("blog", "index.jonj");
__HALT_COMPILER(); ?>');
$phar['index.jonj'] = '<?jonj if (!file_exists("config.xml")) {
	include "install.jonj";
	exit;
}
var_dump(file_get_contents("config.xml"));
?>';
$phar['install.jonj'] = '<?jonj echo "install\n"; ?>';

?>
