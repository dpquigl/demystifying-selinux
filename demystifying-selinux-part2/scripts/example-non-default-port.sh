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

# The script assumes that apache is already started
netstat -alpZ | grep httpd

# Change our config to listen on port 10321 instead of 80
sed -i 's/80/10321/g' /etc/httpd/conf/httpd.conf

# Try to restart the service
service httpd restart

# Failure Lets see how ports are labeled
semanage port -l | less

# Lets add a mapping for our new port
semanage port -a -t http_port_t -p tcp 10321

# Show the new mapping
semanage port -l | grep 10321

# Restart the service
service httpd restart

# Check out httpd running on our new port
netstat -alpZ | grep httpd

# Change the config back
sed -i 's/10321/80/g' /etc/httpd/conf/httpd.conf

# Remove our mapping to 10321
semanage port -d -p tcp 10321

# Restart the service
service httpd restart

# Show httpd back to normal
netstat -alpZ | grep httpd

set +v
trap debug
exit 0
