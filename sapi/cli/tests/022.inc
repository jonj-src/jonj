<?jonj

ob_start();
var_dump(STDIN);

$fd = fopen("jonj://stdin","r");
var_dump($fd);

$client_socket = stream_socket_accept($fd);

$data = ob_get_clean();
fwrite($client_socket, $data);

?>
