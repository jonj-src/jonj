<?jonj
// DO NOT dl load extension
//if (!extension_loaded("xml")) {
//  $dlext = (substr(JONJ_OS, 0, 3) == "WIN") ? ".dll" : ".so";
//  @dl("xml$dlext");
//}
if (!extension_loaded("xml")) {
  die('skip xml extension not available');
}
?>
