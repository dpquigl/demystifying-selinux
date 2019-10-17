#!/bin/bash

# Script Template Courtesy of: Bernie Karmilowicz <karmilow@intencorp.com>
# Script Name:	example.sh
# Programmer:	Dave Quigley <dpquigl@davequigley.com>
# Date:		2013-07-17
# Purpose:	Provides a working example of a script to facilite a presentation involving the entry of shell commands.
# Usage:	./example-incorrect-label.sh 2>&1 | grep -Ev "^read$"


# The following commands setup a working environment, and as such are not intended as part of the presentation so will not be displayed.
ORIG_DIR=`pwd`

# Begin displaying and stepping-thru scripted commands.
trap read debug
set -v

# Administrator creates a file or extracts an archive to /tmp
echo "Hello from www.example-labels.com" > ~/index.html

# We display the label of index.html to see what it is.
ls -Z ~/index.html

# We move index.html into the site's DocumentRoot folder
mv ~/index.html /var/www/html

# Attempt to open the page in firefox
firefox http://www.example-labels.com > /dev/null 2>&1 &

# Wait for firefox to start.

# The display failed? Lets see why. Display the label on index.html again
ls -Z /var/www/html/index.html

# Display the label on the DocumentRoot folder
ls -Z /var/www/html

# Display what the policy believes index.html should be labeled.
matchpathcon /var/www/html/index.html

# Restore the labeling of files under /var/www/html
restorecon -Rvv /var/www/html

set +v
trap debug
# Cease displaying and stepping-thru scripted commands.

# The following commands restore the original environment.
cd ${ORIG_DIR}

exit 0
