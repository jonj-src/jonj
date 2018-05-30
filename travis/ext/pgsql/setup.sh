#!/bin/bash
echo '                         
<?jonj $conn_str .= " user=postgres"; ?>' >> "./ext/pgsql/tests/config.inc"
psql -c 'create database test;' -U postgres