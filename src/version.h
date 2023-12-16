// Copyright (c) 2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef DIMINUTIVECOIN_VERSION_H
#define DIMINUTIVECOIN_VERSION_H

#include "clientversion.h"
#include <string>

//
// client versioning
//

static const int CLIENT_VERSION =
                           1000000 * CLIENT_VERSION_MAJOR
                         +   10000 * CLIENT_VERSION_MINOR
                         +     100 * CLIENT_VERSION_REVISION
                         +       1 * CLIENT_VERSION_BUILD;

extern const std::string CLIENT_NAME;
extern const std::string CLIENT_BUILD;
extern const std::string CLIENT_DATE;

//
// database format versioning
//
static const int DATABASE_VERSION = 70001;

//
// network protocol versioning
//

static const int PROTOCOL_VERSION = 60008;

// intial proto version, to be increased after version/verack negotiation
static const int INIT_PROTO_VERSION = 100;

// disconnect from peers older than this proto version
static const int MIN_PEER_PROTO_VERSION = 60007;

// nTime field added to CAddress, starting with this version;
// if possible, avoid requesting addresses nodes older than this
static const int CADDR_TIME_VERSION = 31402;

// only request blocks from nodes outside this range of versions
static const int NOBLKS_VERSION_START = 60000;
static const int NOBLKS_VERSION_END = 60006;

// hard cutoff time for legacy/future network connections
static const int64_t HRD_LEGACY_CUTOFF = 9993058800; // OFF (NOT TOGGLED)
static const int64_t HRD_FUTURE_CUTOFF = 9993058800; // OFF (NOT TOGGLED)

// hard cutoff block height for min-peer relay/sync
// Disabled as of version 1.0.1.3 , re-enabled as needed for future upgrades
static const int BLOCKHEIGHT_CUTOFF = 9999990;

// BIP 0031, pong message, is enabled for all versions AFTER this one
static const int BIP0031_VERSION = 60000;

// "mempool" command, enhanced "getdata" behavior starts with this version:
static const int MEMPOOL_GD_VERSION = 60000;

// "demi-nodes" command, enhanced "getdata" behavior starts with this version:
static const int DEMINODE_VERSION = 60007;

// reject blocks with non-canonical signatures starting from this version
static const int CANONICAL_BLOCK_SIG_VERSION = 60000;
static const int CANONICAL_BLOCK_SIG_LOW_S_VERSION = 60000;

#endif
