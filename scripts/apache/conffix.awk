# $Id$

/^[ \t]*jonj3_*/ {
	jonjcommand=substr($1,6)
	jonjvalue=tolower($2)
	print "<IfModule mod_jonj3.c>"
	print $0
	print "</IfModule>"
	print "<IfModule mod_jonj4.c>"
	if (jonjvalue=="on") {
		print "jonj_admin_flag " jonjcommand " on"
	} else  if (jonjvalue=="off") {
		print "jonj_admin_flag " jonjcommand " off"
	} else {
		print "jonj_admin_value " jonjcommand " " substr($0,index($0,$1)+length($1)+1)
	}
	print "</IfModule>"
}

! /^[ \t]*jonj3_*/ {
	print $0
}

