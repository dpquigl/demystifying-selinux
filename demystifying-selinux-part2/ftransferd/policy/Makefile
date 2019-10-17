POLICYSRC= ftransferd.te ftransferd.if ftransferd.fc

all: ftransferd.pp

ftransferd.pp: 
	make -f /usr/share/selinux/devel/Makefile ftransferd.pp

clean:
	rm -rf ftransferd.pp tmp

install:
	semodule -i ftransferd.pp

uninstall:
	semodule -d ftransferd.pp
