#!/bin/sh
export DEGATE_HOME=gui;
gdb -x .gdb_commands -q $DEGATE_HOME/degate_bin


