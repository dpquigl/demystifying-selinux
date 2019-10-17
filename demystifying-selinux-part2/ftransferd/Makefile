all: ftransferd.pp ftransfer ftransferd

ftransferd.pp: policy/ftransferd.te policy/ftransferd.if policy/ftransferd.fc
	make -f /usr/share/selinux/devel/Makefile ftransferd.pp

ftransferd: ftransferd.c
	gcc -g -o ftransferd ftransferd.c

ftransfer: ftransfer.c
	gcc -g -o ftransfer ftransfer.c

clean:
	rm ftransfer ftransferd
	rm -rf ftransferd.pp tmp

#Installation/uninstallation rules
install_policy: ftransferd.pp
	semodule -i ftransferd.pp

install_ftransferd: ftransferd
	install ftransferd.init /etc/rc.d/init.d/ftransferd
	install ftransfer /usr/local/bin/ftransfer
	install ftransferd /usr/local/bin/ftransferd

install: install_policy install_ftransferd

uninstall_policy:
	semodule -r ftransferd

uninstall_ftransferd:
	rm -rf /etc/init.d/ftransferd /usr/local/bin/ftransferd /usr/local/bin/ftransfer 

uninstall: uninstall_ftransferd uninstall_policy
