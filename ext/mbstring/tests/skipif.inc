<?jonj
// This script prints "skip" if condition does not meet.

/*
if (!extension_loaded("mbstring") && ini_get("enable_dl")) {
  $dlext = (substr(JONJ_OS, 0, 3) == "WIN") ? ".dll" : ".so";
  @dl("mbstring$dlext");
}
*/

if (!extension_loaded("mbstring")) {
    die("skip\n");
}
?>
