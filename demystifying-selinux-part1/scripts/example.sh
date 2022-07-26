#!/bin/bash

# Script Template Courtesy of: Bernie Karmilowicz <karmilow@intencorp.com>
# Script Name:	example.sh
# Programmer:	Dave Quigley <dpquigl@davequigley.com>
# Date:		2013-07-17
# Purpose:	Provides a working example of a script to facilite a presentation involving the entry of shell commands.
# Usage:	example.sh 2>&1 | grep -Ev "^read$"


# The following commands setup a working environment, and as such are not intended as part of the presentation so will not be displayed.
ORIG_DIR=`pwd`

# Begin displaying and stepping-thru scripted commands.
trap read debug
set -v


# The following commands are part of the presentation, so will be stepped-thru and displayed.
pwd
cd /tmp
pwd
ls -l


# Cease displaying and stepping-thru scripted commands.
set +v
trap debug

# The following commands restore the original environment.
cd ${ORIG_DIR}

exit 0
