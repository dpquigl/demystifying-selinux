#!/bin/bash

# Script Template Courtesy of: Bernie Karmilowicz <karmilow@intencorp.com>
# Script Name:	example.sh
# Programmer:	Dave Quigley <dpquigl@davequigley.com>
# Date:		2022-08-29
# Purpose:	Provides a working example of a script to facilite a presentation involving the entry of shell commands.
# Usage:	./example-ftransfer.sh 2>&1 | grep -Ev "^read$"


# Begin displaying and stepping-thru scripted commands.
trap read debug
set -v

# Create public_html in sedemo
mkdir ~sedemo/public_ftransfer

# Create index.html in public_html
echo "Hello From sedemo's home directory" > ~sedemo/public_ftransfer/hello.txt

# Fixup DAC permissions
chown -R sedemo:sedemo ~sedemo/public_ftransfer
chmod o+rx /home/sedemo
chmod o+rx ~sedemo/public_ftransfer

# Look at label on the directory
ls -Z ~sedemo | grep public_ftransfer

# Restore the label on public_html
chcon -R -t ftransferd_content_t ~sedemo/public_ftransfer

# Look at label on the directory
ls -Z ~sedemo | grep public_ftransfer

# Temporarially cause ftrasferd to run in permissive mode
semanage permissive -a ftransferd_t

# Show that ftransferd is now in a permissive domain
semanage permissive -l

# Show the path traversal exploit in ftransferd
ftransfer sedemo ../../../../etc/passwd 20000 127.0.0.1 exfil

# Show the contents of the transfer
cat exfil

# Remove ftransferd_t from the permissive domain list
semanage permissive -d ftransferd_t

# Try again but this time with SELinux enforcing on the ftrandferd_t domain
ftransfer sedemo ../../../../etc/passwd 20000 127.0.0.1 exfil

# Show the contents of the transfer
cat exfil

# Try again but this time with SELinux enforcing on the ftrandferd_t domain
ftransfer sedemo hello.txt 20000 127.0.0.1 hello.txt

# Show we can really transfer the files we want
cat hello.txt

set +v
trap debug

exit 0
