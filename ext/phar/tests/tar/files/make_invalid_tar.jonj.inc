<?jonj
include dirname(__FILE__) . '/tarmaker.jonj.inc';
class corrupter extends tarmaker {
function close()
{
	fwrite($this->tmp, (binary)'oopsie');
	fclose($this->tmp);
}
}