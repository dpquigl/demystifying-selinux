#!/bin/bash

# Script Template Courtesy of: Bernie Karmilowicz <karmilow@intencorp.com>
# Script Name:	example.sh
# Programmer:	Dave Quigley <dpquigl@davequigley.com>
# Date:		2013-07-17
# Purpose:	Provides a working example of a script to facilite a presentation involving the entry of shell commands.
# Usage:	./example-booleans.sh 2>&1 | grep -Ev "^read$"


# Begin displaying and stepping-thru scripted commands.
trap read debug
set -v

# Create public_html in sedemo
mkdir ~sedemo/public_html

# Create index.html in public_html
echo "Hello From sedemo's home directory" > ~sedemo/public_html/index.html

# Fixup DAC permissions
chown -R sedemo:sedemo ~sedemo/public_html
chmod o+rx /home/sedemo
chmod o+rx ~sedemo/public_html

# Look at label on the directory
ls -Z ~sedemo | grep public_html

# Restore the label on public_html
restorecon -Rvv ~sedemo/public_html

# Look for httpd booleans
getsebool -a | grep httpd

# Set appropriate booleans
setsebool -P httpd_enable_homedirs=1 httpd_read_user_content=1

# Show the booleans are set
getsebool -a | grep httpd

set +v
trap debug

exit 0
