#!/bin/bash

launchctl stop $(launchctl list | awk '/com.docker.docker/ {print $3}') 2> /dev/null
rm -rf ~/.docker
rm -rf ~/Library/Containers
mkdir -p ~/goinfre/Containers
rm -rf ~/goinfre/Containers/*
ln -sf ~/goinfre/Containers ~/Library/
