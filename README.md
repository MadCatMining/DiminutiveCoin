Diminutivecoin
=====================================

https://diminutivecoin.com

What is Diminutivecoin?
----------------

Diminutivecoin is a decentralised digital currency with near-instant transaction speeds and negligible transaction fees built upon Proof of Stake 3.1 (PoSV3, BPoS) as
introduced by the Blackcoin development team.

For more information about Diminutivecoin itself, see https://diminutivecoin.com.

What is Diminutivecoin?
----------------

Diminutivecoin is the name of open source software which enables the use of this currency. It takes Diminutivecoin to the next level by building upon
Bitcoin Core 0.13.2 with some patches from newer Bitcoin Core versions to offer performance enhancements, wider compatibility with third party services and a more advanced base.

For more information, as well as an immediately useable, binary version of the Diminutivecoin software, see https://diminutivecoin.com.

License
-------

Diminutivecoin is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/MadCatMining/DiminutiveCoin/tags) are created
regularly to indicate new official, stable release versions of Diminutivecoin.

Change log can be found in [CHANGELOG.md](CHANGELOG.md).

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).

The best place to get started is to join Diminutivecoin Discord: https://discord.gg/tX85tUf6d9

Testing
-------

Testing and code review might be the bottleneck for development. Please help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](/doc/unit-tests.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`

There are also [regression and integration tests](/qa) of the RPC interface, written
in Python, that are run automatically on the build server.
These tests can be run (if the [test dependencies](/qa) are installed) with: `qa/pull-tester/rpc-tests.py`

The Travis CI system makes sure that every pull request is built for Windows, Linux, and OS X, and that unit/sanity tests are run automatically.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.

Branches
-------

### develop
The develop branch is typically used by developers as the main branch for integrating new features and changes into the codebase.
Pull requests should always be made to this branch (except for critical fixes), and might possibly break the code.
The develop branch is considered an unstable branch, as it is constantly updated with new code, and it may contain bugs or unfinished features. It is not guaranteed to work properly on any system.

### master
The master branch gets latest updates from the stable branch.
However, it may contain experimental features and should be used with caution.

### 13.0
The release branch for Diminutivecoin 13.0.x. It is intended to contain stable and functional code that has been thoroughly tested and reviewed.

