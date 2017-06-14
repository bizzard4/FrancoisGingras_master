#!/bin/bash

gcc server.c -lrt -o server
gcc client.c -lrt -o client