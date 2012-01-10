#!/bin/sh

# First we set environment variables.

# This is mandatory, because degate must know where all the glade files and icons are.
export DEGATE_HOME=gui;

# Set the global image tile cache size in MB.
# Each image tile has a dimension of 1024x1024 pixels. Each pixel is an RGBA value.
# That is a single image tile has 4MB. You should adjust the cache size, that at
# least a whole horizotnal row of image tiles fit into the cache. E. g. if your
# project usually has a horizontal dimension of 10000 pixel, you need around 10
# tiles for a tile row. That are around 40 MB cache size. 
# If you have enough memory, you can multiply this value by the number of layers
# you normally use. 
export DEGATE_CACHE_SIZE=256

# Set the temp directory.
export DEGATE_TEMP=/tmp

# Set the URI pattern for the collaboration server.
export DEGATE_SERVER_URI_PATTERN=http://degate.org/cgi-bin/xmlrpc-server-cgi.pl?channel=%1%

# Launch degate.
$DEGATE_HOME/degate_bin
