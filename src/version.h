// Copyright (c) 2012-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_VERSION_H
#define BITCOIN_VERSION_H

/**
 * network protocol versioning
 */

static const int PROTOCOL_VERSION = 70016;

//! initial proto version, to be increased after version/verack negotiation
static const int INIT_PROTO_VERSION = 209;

//! In this version, 'getheaders' was introduced.
static const int GETHEADERS_VERSION = 31800;

//! reject blocks with non-canonical signatures starting from this version
static const int CANONICAL_BLOCK_SIG_VERSION = 60016;
static const int CANONICAL_BLOCK_SIG_LOW_S_VERSION = 60018;

//! disconnect from peers older than this proto version
static const int MIN_PEER_PROTO_VERSION = 70016;

//! nTime field added to CAddress, starting with this version;
//! if possible, avoid requesting addresses nodes older than this
static const int CADDR_TIME_VERSION = 31402;

//! BIP 0031, pong message, is enabled for all versions AFTER this one
static const int BIP0031_VERSION = 60000;

//! "mempool" command, enhanced "getdata" behavior starts with this version
static const int MEMPOOL_GD_VERSION = 60002;

//! "filter*" commands are disabled without NODE_BLOOM after and including this version
static const int NO_BLOOM_VERSION = 70011;

//! "sendheaders" command and announcing blocks with headers starts with this version
static const int SENDHEADERS_VERSION = 70012;

//! "feefilter" tells peers to filter invs to you by fee starts with this version
static const int FEEFILTER_VERSION = 70013;

//! short-id-based block download starts with this version
static const int SHORT_IDS_BLOCKS_VERSION = 70014;

//! not banning for invalid compact blocks starts with this version
static const int INVALID_CB_NO_BAN_VERSION = 70015;

#endif // BITCOIN_VERSION_H
