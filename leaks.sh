#!/bin/bash

# Loop to check for leaks every 2 seconds
while true
do
    leaks -quiet webserv
    sleep 2
done
