# Rego

## Introduction

Rego is a library and a set of tools for interacting with the Rego600 series
heat pump controller.

* `src/protocol` is a small library that encapsulates the serial protocol (or the
  wire format to be more precise)

* `src/regoctl` implements a small command line utility for reading and writing
  Rego registers

* `src/regod` implements a daemon that acts as a proxy by talking to Rego and by
  providing a D-Bus interface that allows for multiple clients to share the
  serial line and interact with Rego


## Dependencies

* `boost::program_options`
* `boost::system`
* `boost::optional`
* [log4cxx](https://logging.apache.org/log4cxx/index.html)
* [sdbusplus](https://github.com/openbmc/sdbusplus)


### Tests

* [Ruby Version Manager (RVM)](https://rvm.io/)
* `socat`
* The Ruby gems listed in Gemfile


## Setting up

### Install Cucumber

* [Install RVM](https://rvm.io/rvm/install)

```bash
$ gpg --keyserver hkp://keys.gnupg.net --recv-keys 409B6B1796C275462A1703113804BB82D39DC0E3 7D2BAF1CF37B13E2069D6956105BD0E739499BDB
$ \curl -sSL https://get.rvm.io | bash -s stable
```

* Install Ruby

```bash
$ rvm install 2.3.0
```

* Use the Ruby installation

```bash
$ rvm use 2.3.0
```

* Create a dedicated gem set

```bash
$ rvm gemset create rego
```

* Install Bundler

```bash
$ gem install bundler --no-rdoc --no-ri
```

* Activate RVM by re-entering the project root directory so that RVM reads
  .ruby-version)

```bash
$ cd features; cd -
```

* Install Ruby gems

```bash
$ bundle install
```


## Building

* Create and enter a build directory

```bash
$ mkdir build; cd build
```

* Configure the build

```bash
$ cmake -G Ninja -DCMAKE_INSTALL_PREFIX=/path/to/your/rego/clone ..
```

* Build

```bash
$ ninja
```

* Install binaries in order to run tests

```bash
$ ninja install
```


## Running tests

* Use the dedicated gem set

```bash
$ rvm use 2.3.0@rego
```

* Start the test runner in the background

```bash
$ ./features/steps/steps &
```

* Run features with Cucumber

```bash
$ cucumber features/
```


## Notes

### Building against sdbusplus installed in /usr/local

```bash
$ PKG_CONFIG_PATH=/usr/local/lib/pkgconfig cmake -G Ninja ..
```


### (Re-)generating the D-Bus bindings

* Clone sdbusplus

```bash
$ git clone https://github.com/openbmc/sdbusplus.git
```


* Apply the following patch to sdbusplus

```
commit 2624e2efb67727fa861c48246617991789a3489b (HEAD -> disable-examples-tests-tools)
Author: Hannu Lounento <hannu.lounento@iki.fi>
Date:   Fri Feb 9 10:10:15 2018 +0200

    [dbg] Don't build examples, tests, and tools
    
    This is a quick and dirty solution to workaround some build problems in
    my environment. The build is needed in order to process the Mako
    templates for use by tools/sdbusc++.
    
    Signed-off-by: Hannu Lounento <hannu.lounento@iki.fi>

diff --git a/Makefile.am b/Makefile.am
index 037640c..6946f6a 100644
--- a/Makefile.am
+++ b/Makefile.am
@@ -43,7 +43,5 @@ libsdbusplus_la_CXXFLAGS = $(SYSTEMD_CFLAGS)
 pkgconfiglibdir = ${libdir}/pkgconfig
 pkgconfiglib_DATA = sdbusplus.pc
 
-SUBDIRS = . tools example test
 else
-SUBDIRS = . tools
 endif # WANT_LIBSDBUSPLUS
```


* Build sdbusplus

```bash
$ mkdir build && cd build
$ ../configure && make
$ cd -
```

* Generate the bindings

```bash
$ ./virtualenv/bin/activate
(virtualenv) $ ./tools/sdbus++ -r /path/to/rego/api -t build/tools/sdbusplus/templates interface server-header fi.iki.halo.Rego1.Sensors > /path/to/rego/src/regod/fi/iki/halo/Rego1/Sensors/server.hpp
(virtualenv) $ ./tools/sdbus++ -r /path/to/rego/api -t build/tools/sdbusplus/templates interface server-cpp fi.iki.halo.Rego1.Sensors > /path/to/rego/src/regod/fi/iki/halo/Rego1/Sensors/server.cpp
```
