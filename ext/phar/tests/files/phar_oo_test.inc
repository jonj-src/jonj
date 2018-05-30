<?jonj

ini_set('date.timezone', 'GMT');

$tname = basename(current(get_included_files()), ".jonj");
$fname = dirname(__FILE__) . "/$tname.phar.jonj";
$pname = 'phar://' . $fname;
$file = (binary)'<?jonj include "' . $pname . '/a.jonj"; __HALT_COMPILER(); ?>';

$files = array();

if (!isset($pharconfig)) $pharconfig = 0;

switch($pharconfig)
{
	default:
	case 0:
		$files['a.jonj']   = '<?jonj echo "This is a.jonj\n"; ?>';
		$files['b.jonj']   = '<?jonj echo "This is b.jonj\n"; ?>';
		$files['b/c.jonj'] = '<?jonj echo "This is b/c.jonj\n"; ?>';
		$files['b/d.jonj'] = '<?jonj echo "This is b/d.jonj\n"; ?>';
		$files['e.jonj']   = '<?jonj echo "This is e.jonj\n"; ?>';
		break;
	case 1:
		$files['a.csv'] =<<<EOF
1,2,3
2,a,b
3,"c","'e'"
EOF;
		break;
	case 2:
		$files['a.csv'] =<<<EOF
1,2,3
2,a,b
3,"c","'e'"
4
5,5

7,777
EOF;
		break;
	case 3:
		$files['a.jonj'] = '<?jonj echo new new class;';
		break;
}

$ftime = mktime(12, 0, 0, 3, 1, 2006);
include 'phar_test.inc';

?>