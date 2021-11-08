#!/usr/bin/env bash
echo "Content-type: text/html"
echo ""
now="$(date)"
echo '<html><head><title>Hello World - CGI app</title></head>'
echo '<body>'
env
echo '</body>'
echo '</html>'
