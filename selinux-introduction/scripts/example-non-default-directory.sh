#!/bin/bash

# Script Template Courtesy of: Bernie Karmilowicz <karmilow@intencorp.com>
# Script Name:	example.sh
# Programmer:	Dave Quigley <dpquigl@davequigley.com>
# Date:		2013-07-17
# Purpose:	Provides a working example of a script to facilite a presentation involving the entry of shell commands.
# Usage:	./example-non-default-port.sh 2>&1 | grep -Ev "^read$"

# Begin displaying and stepping-thru scripted commands.
trap read debug
set -v

# Create our new DocumentRoot for www.example-opt.com
cp -R /var/www/ /opt

# Create our new index.html
echo "Hello From www.example-opt.com" > /opt/www/html/index.html

# Check out the label on /opt/www
ls -Z /opt

# Check out the label on /var/www
ls -Z /var/

# Set the label with chcon
chcon -R -t httpd_sys_content_t /opt/www

# Show a relabel wipes it away
restorecon -Rvv /opt/www

# Set the label the right way
semanage fcontext -a -e /var/www /opt/www

# Show a relabel gets the labels right this time
restorecon -Rvv /opt/www

set +v
trap debug

exit 0
