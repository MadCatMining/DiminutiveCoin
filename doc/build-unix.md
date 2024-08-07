Linux/Unix diminutivecoind: Headless Daemon for DiminutiveCoin
===============================


Build instructions
===================

Compiling DiminutiveCoin daemon on Ubuntu 20.04 LTS
---------------------------
### Note: guide should be compatible with other Ubuntu/Linux versions from 14.04+
(Just copy and paste the text in the boxes into your console)

### Become poweruser
```
sudo -i
```

### Create swap file (if system has less than 2GB of RAM)
```
cd ~; sudo fallocate -l 3G /swapfile; ls -lh /swapfile; sudo chmod 600 /swapfile; ls -lh /swapfile; sudo mkswap /swapfile; sudo swapon /swapfile; sudo swapon --show; sudo cp /etc/fstab /etc/fstab.bak; echo '/swapfile none swap sw 0 0' | sudo tee -a /etc/fstab
```

### Dependencies install
```
cd ~; apt-get install -y ntp git build-essential libssl-dev libdb-dev libdb++-dev libboost-all-dev libqrencode-dev libcurl4-openssl-dev curl libzip-dev; apt-get update -y; apt-get install -y git make automake build-essential libboost-all-dev; apt-get install -y yasm binutils libcurl4-openssl-dev openssl libssl-dev; sudo apt-get install -y libgmp-dev;
```

### Dependencies build and link
```
cd ~; wget http://download.oracle.com/berkeley-db/db-4.8.30.NC.tar.gz; tar zxf db-4.8.30.NC.tar.gz; cd db-4.8.30.NC/build_unix; ../dist/configure --enable-cxx; make; make install; ln -s /usr/local/BerkeleyDB.6.2/lib/libdb-6.2.so /usr/lib/libdb-6.2.so;ln -s /usr/local/BerkeleyDB.6.2/lib/libdb_cxx-6.2.so /usr/lib/libdb_cxx-6.2.so; export BDB_INCLUDE_PATH="/usr/local/BerkeleyDB.6.2/include"; export BDB_LIB_PATH="/usr/local/BerkeleyDB.6.2/lib"; cd ~;
```

### GitHub pull (Source Download)
```
cd ~; git clone https://github.com/CryptoCoderz/DiminutiveCoin
```

### Build DiminutiveCoin daemon
```
cd ~; cd ~/DiminutiveCoin/src; chmod a+x obj; chmod a+x leveldb/build_detect_platform; chmod a+x leveldb; chmod a+x ~/DiminutiveCoin/src; chmod a+x ~/DiminutiveCoin; make -f ~/DiminutiveCoin/src/makefile.unix USE_UPNP=-; cd ~; cp ~/DiminutiveCoin/src/diminutivecoind /usr/local/bin;
```

### Create config file
```
cd ~; sudo ufw allow 49139/tcp; sudo ufw allow 49122/tcp; sudo mkdir ~/.diminutivecoin; cat << "CONFIG" >> ~/.diminutivecoin/diminutivecoin.conf
listen=1
server=1
daemon=1
deminodes=1
demimaxdepth=200
maxconnections=500
testnet=0
rpcuser=dimiuser
rpcpassword=SomeCrazyVeryVerySecurePasswordHere
rpcport=49122
port=49139
rpcconnect=127.0.0.1
rpcallowip=127.0.0.1
addnode=75.119.140.224:49139
addnode=38.242.255.229:49139
addnode=176.57.189.38:49139

CONFIG
chmod 700 ~/.diminutivecoin/diminutivecoin.conf; chmod 700 ~/.diminutivecoin; ls -la ~/.diminutivecoin
```

### Run DiminutiveCoin daemon
```
cd ~; diminutivecoind
```

### Get Data DiminutiveCoin daemon
```
cd ~; diminutivecoind getinfo
```

### Troubleshooting
### for basic troubleshooting run the following commands when compiling:
### this is for minupnpc errors compiling
```
make clean -f makefile/makefile.unix USE_UPNP=-
make -f makefile/makefile.unix USE_UPNP=-
```


Dependencies Used
-----------------

 Library     Purpose           Description
 -------     -------           -----------
 libssl      SSL Support       Secure communications
 libdb       Berkeley DB       Blockchain & wallet storage
 libboost    Boost             C++ Library
 miniupnpc   UPnP Support      Optional firewall-jumping support
 libqrencode QRCode generation Optional QRCode generation

Note that libexecinfo should be installed, if you building under BSD systems. 
This library provides backtrace facility.

miniupnpc may be used for UPnP port mapping.  It can be downloaded from
http://miniupnp.tuxfamily.org/files/.  UPnP support is compiled in and
turned off by default.  Set USE_UPNP to a different value to control this:
 USE_UPNP=-    No UPnP support - miniupnp not required
 USE_UPNP=0    (the default) UPnP support turned off by default at runtime
 USE_UPNP=1    UPnP support turned on by default at runtime

libqrencode may be used for QRCode image generation. It can be downloaded
from http://fukuchi.org/works/qrencode/index.html.en, or installed via
your package manager. Set USE_QRCODE to control this:
 USE_QRCODE=0   (the default) No QRCode support - libqrcode not required
 USE_QRCODE=1   QRCode support enabled

Licenses of statically linked libraries:
 Berkeley DB   New BSD license with additional requirement that linked
               software must be free open source
 Boost         MIT-like license
 miniupnpc     New (3-clause) BSD license

Versions used in this release:
 GCC           8.1.0
 OpenSSL       1.0.2u - 1.2.X
 Berkeley DB   4.8.30.NC
 Boost         1.74.0
 miniupnpc     2.1.20190824


Security
--------
To help make your DiminutiveCoin installation more secure by making certain attacks impossible to
exploit even if a vulnerability is found, you can take the following measures:

* Position Independent Executable
    Build position independent code to take advantage of Address Space Layout Randomization
    offered by some kernels. An attacker who is able to cause execution of code at an arbitrary
    memory location is thwarted if he doesn't know where anything useful is located.
    The stack and heap are randomly located by default but this allows the code section to be
    randomly located as well.

    On an Amd64 processor where a library was not compiled with -fPIC, this will cause an error
    such as: "relocation R_X86_64_32 against ......' can not be used when making a shared object;"

    To build with PIE, use:
    make -f makefile.unix ... -e PIE=1

    To test that you have built PIE executable, install scanelf, part of paxutils, and use:
    scanelf -e ./DiminutiveCoin

    The output should contain:
     TYPE
    ET_DYN

* Non-executable Stack
    If the stack is executable then trivial stack based buffer overflow exploits are possible if
    vulnerable buffers are found. By default, DiminutiveCoin should be built with a non-executable stack
    but if one of the libraries it uses asks for an executable stack or someone makes a mistake
    and uses a compiler extension which requires an executable stack, it will silently build an
    executable without the non-executable stack protection.

    To verify that the stack is non-executable after compiling use:
    scanelf -e ./DiminutiveCoin

    the output should contain:
    STK/REL/PTL
    RW- R-- RW-

    The STK RW- means that the stack is readable and writeable but not executable.
