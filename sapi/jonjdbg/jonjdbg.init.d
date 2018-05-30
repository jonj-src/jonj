################################################################
# File:         /etc/init.d/jonjdbg                             #
# Author:       krakjoe                                        #
# Purpose:      Daemonize jonjdbg automatically on boot         #
# chkconfig:    2345    07 09                                  #
# description:  Starts, stops and restarts jonjdbg daemon       #
################################################################
LOCKFILE=/var/lock/subsys/jonjdbg
PIDFILE=/var/run/jonjdbg.pid
STDIN=4000
STDOUT=8000
################################################################
# Either set path to jonjdbg here or rely on jonjdbg in ENV/PATH #
################################################################
if [ "x${JONJDBG}" == "x" ]; then
	JONJDBG=$(which jonjdbg 2>/dev/null)
fi
################################################################
# Options to pass to jonjdbg upon boot                          #
################################################################
OPTIONS=
LOGFILE=/var/log/jonjdbg.log
################################################################
#     STOP EDITING STOP EDITING STOP EDITING STOP EDITING      #
################################################################
. /etc/rc.d/init.d/functions
RETVAL=1
################################################################
insanity()
{
	if [ "x${JONJDBG}" == "x" ]; then
		JONJDBG=$(which jonjdbg 2>>/dev/null)
		if [ $? != 0 ]; then
			echo -n $"Fatal: cannot find jonjdbg ${JONJDBG}"
			echo_failure
			echo
			return 1
		fi
	else
		if [ ! -x ${JONJDBG} ]; then
			echo -n $"Fatal: cannot execute jonjdbg ${JONJDBG}"
			echo_failure
			echo
			return 1
		fi
	fi
	
	return 0
}

start()
{
	insanity

	if [ $? -eq 1 ]; then
		return $RETVAL
	fi

	echo -n $"Starting: jonjdbg ${OPTIONS} on ${STDIN}/${STDOUT} "
	nohup ${JONJDBG} -l${STDIN}/${STDOUT} ${OPTIONS} 2>>${LOGFILE} 1>/dev/null </dev/null &
	PID=$!
	RETVAL=$?
	if [ $RETVAL -eq 0 ]; then
		echo $PID > $PIDFILE
		echo_success
	else
		echo_failure
	fi
	echo
	[ $RETVAL = 0 ] && touch ${LOCKFILE}
	return $RETVAL
}

stop()
{
	insanity

	if [ $? -eq 1 ]; then
		return $RETVAL
	fi

	if [ -f ${LOCKFILE} ] && [ -f ${PIDFILE} ]
	then
		echo -n $"Stopping: jonjdbg ${OPTIONS} on ${STDIN}/${STDOUT} "
		kill -s TERM $(cat $PIDFILE)
		RETVAL=$?
		if [ $RETVAL -eq 0 ]; then
			echo_success
		else
			echo_failure
		fi
		echo
		[ $RETVAL = 0 ] && rm -f ${LOCKFILE} ${PIDFILE}
	else
		echo -n $"Error: jonjdbg not running"
		echo_failure
		echo
		[ $RETVAL = 1 ]
	fi
	return $RETVAL
}
##################################################################
case "$1" in
	start)
	start
	;;
	stop)
	stop
	;;
	status)
	status $JONJDBG
	;;
	restart)
	$0 stop
	$0 start
	;;
	*)
	echo "usage: $0 start|stop|restart|status"
	;;
esac
###################################################################
exit $RETVAL
