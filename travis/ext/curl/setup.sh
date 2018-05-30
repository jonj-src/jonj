#!/bin/bash
export JONJ_CURL_HTTP_REMOTE_SERVER="http://localhost:8080"
cd ./ext/curl/tests/responder
jonj -S localhost:8080 &
cd -
