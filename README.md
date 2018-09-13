NOVA README
=================

Welcome to NOVA!

NOVA is a smart contract platform.

Engine:
-----------------
	novad       -- NOVA Backend Daemon

CLP:
-----------------
	nova        -- NOVA client
	novabp      -- NOVA client backend process

Building Prerequisites:
-----------------
	#install boost 1.58
	sudo apt-get install libboost1.58-all-dev

	#install spidermonkey 1.85
	# 1. install autoconf213
	sudo apt-get install autoconf2.13

	# 2 download spidermonkey 1.85
	# 2.1 download source code
	http://ftp.mozilla.org/pub/mozilla.org/js/js185-1.0.0.tar.gz
	# 2.2.1 compile and install
	tar -xvf js185-1.0.0.tar.gz
	cd js/src
	autoconf-2.13
	make build_OPT.OBJ
	cd build_OPT.OBJ
	../configure
	make
	sudo make install
	# 2.2.2 or install spidermonkey lib binary
	sudo apt-get install libmozjs185-1.0

	#install openssl
	sudo apt-get install libssl-dev

	#install nspr
	sudo apt-get install libnspr4-dev

Building Engine:
-----------------
	 cd %novachains_home%  
	 cmake .  
	 make  

Building CLP:
-----------------

Package RPM:
-----------------

Running:
-----------------

Documentation:
-----------------
[NOVA Home Page](http://www.novachains.io/)

Restrictions:
-----------------
	- NOVA officially supports x86_64 Linux build on CentOS, Redhat, SUSE and Ubuntu.

License:
-----------------
	Most NOVA source files are made available under the terms of the
	GNU Affero General Public License (AGPL). See individual files for details.
	All source files for clients, drivers and connectors are released
	under Apache License v2.0.
