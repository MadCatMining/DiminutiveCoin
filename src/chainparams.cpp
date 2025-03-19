// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "base58.h"
#include "chainparams.h"
#include "consensus/merkle.h"
#include "dstencode.h"

#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"
#include "arith_uint256.h"

using namespace std;

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    // Genesis block

    // MainNet:

    //CBlock(hash=000001faef25dec4fbcf906e6242621df2c183bf232f263d0ba5b101911e4563, ver=1, hashPrevBlock=0000000000000000000000000000000000000000000000000000000000000000, hashMerkleRoot=12630d16a97f24b287c8c2594dda5fb98c9e6c70fc61d44191931ea2aa08dc90, nTime=1393221600, nBits=1e0fffff, nNonce=164482, vtx=1, vchBlockSig=)
    //  Coinbase(hash=12630d16a9, nTime=1393221600, ver=1, vin.size=1, vout.size=1, nLockTime=0)
    //    CTxIn(COutPoint(0000000000, 4294967295), coinbase 00012a24323020466562203230313420426974636f696e2041544d7320636f6d6520746f20555341)
    //    CTxOut(empty)
    //  vMerkleTree: 12630d16a9

    // TestNet:

    //CBlock(hash=0000724595fb3b9609d441cbfb9577615c292abf07d996d3edabc48de843642d, ver=1, hashPrevBlock=0000000000000000000000000000000000000000000000000000000000000000, hashMerkleRoot=12630d16a97f24b287c8c2594dda5fb98c9e6c70fc61d44191931ea2aa08dc90, nTime=1393221600, nBits=1f00ffff, nNonce=216178, vtx=1, vchBlockSig=)
    //  Coinbase(hash=12630d16a9, nTime=1393221600, ver=1, vin.size=1, vout.size=1, nLockTime=0)
    //    CTxIn(COutPoint(0000000000, 4294967295), coinbase 00012a24323020466562203230313420426974636f696e2041544d7320636f6d6520746f20555341)
    //    CTxOut(empty)
    //  vMerkleTree: 12630d16a9

    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.nTime = nTime;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 0 << CScriptNum(42) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Kremlin Chokes YouTube Service, but Russians Find Ways Around It";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nMaxReorganizationDepth = 500;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimitV2 = uint256S("000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nTargetTimespan = 16 * 60; // 16 mins
        consensus.nTargetSpacingV1 = 60;
        consensus.nTargetSpacing = 64;
        consensus.BIP34Height = 20;
        consensus.BIP34Hash = uint256();
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.fPoSNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nTargetTimespan / nTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        consensus.nProtocolV1RetargetingFixedTime = 1738340500;
        consensus.nProtocolV2Time = 1738340500;
        consensus.nProtocolV3Time = 1738340500;
        consensus.nProtocolV3_1Time = 1738340500;
        consensus.nLastPOWBlock = 0x7fffffff;
        consensus.nStakeTimestampMask = 0xf; // 15
        consensus.nCoinbaseMaturity = 100;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000000000c9014d493f4dbb"); // block 43000

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x22;
        pchMessageStart[1] = 0x1b;
        pchMessageStart[2] = 0x2f;
        pchMessageStart[3] = 0x57;
        nDefaultPort = 49577;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1738340500, 3031433, 0x1e0fffff, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        
        assert(consensus.hashGenesisBlock == uint256S("0x0000051711c49b2374ba7c506cc089032492c618bb3a3227a39a2f392822785c"));
        assert(genesis.hashMerkleRoot == uint256S("0x2a98e99882ffffbdec123030f141b1df946afe4f3a98a1c507a3f414944c458d"));

        vSeeds.push_back(CDNSSeedData("dns3.diminutivecoin.com", "dns3.diminutivecoin.com"));
        vSeeds.push_back(CDNSSeedData("dns4.diminutivecoin.com", "dns4.diminutivecoin.com"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,32);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,30);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1,181);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0xAD)(0x1B)(0x12)(0xA4).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0xE1)(0xA3)(0x2B)(0x3E).convert_to_container<std::vector<unsigned char> >();
        cashaddrPrefix = "dimi";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData) {
                    boost::assign::map_list_of
                    (     0, uint256S("0x0000051711c49b2374ba7c506cc089032492c618bb3a3227a39a2f392822785c"))   // Genesis
                    ( 10000, uint256S("0x0000004bfd794aca7e3d06d32f963aebb70094cd8c07c6ade1791d9fed2949c4"))   //
                    ( 25000, uint256S("0x0000001b6e343921dbcd23cbb60689ca8bb66e432e4a7a9274ecaeef38de1bbc"))   //
                    ( 43000, uint256S("0x00000194e94e2866697db61c27bfed119a2a910cc04fcb7e44248e1a9cffb777")),  // Pre-release checkpoint
                    1742304469,    // * UNIX timestamp of last checkpoint block
                         43750,    // * total number of transactions between genesis and last checkpoint
                                   //   (the tx=... number in the SetBestChain debug.log lines)
                        1200.0     // * estimated number of transactions per day after checkpoint
        };

        // A vector of p2sh addresses
        vDevFundAddress = { "" };
    }
};
static CMainParams mainParams;

/**
 * Testnet
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nMaxReorganizationDepth = 500;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimitV2 = uint256S("000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nTargetTimespan = 16 * 60; // 16 mins
        consensus.nTargetSpacingV1 = 60;
        consensus.nTargetSpacing = 60;
        consensus.BIP34Height = -1;
        consensus.BIP34Hash = uint256();
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.fPoSNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        consensus.nProtocolV1RetargetingFixedTime = 1738340500;
        consensus.nProtocolV2Time = 1738340500;
        consensus.nProtocolV3Time = 1738340500;
        consensus.nProtocolV3_1Time = 1738340500;
        consensus.nLastPOWBlock = 0x7fffffff;
        consensus.nStakeTimestampMask = 0xf;
        consensus.nCoinbaseMaturity = 10;

        pchMessageStart[0] = 0xcd;
        pchMessageStart[1] = 0xf2;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0xef;
        nDefaultPort = 49566;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000000010"); // block 1

        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1738340500, 34852, 0x1e0fffff, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();

        assert(consensus.hashGenesisBlock == uint256S("0x0000d22b723620dca30ad399d9c5c145416cf1f3e34773f5ce6ffe3623264818"));
        assert(genesis.hashMerkleRoot == uint256S("0x2a98e99882ffffbdec123030f141b1df946afe4f3a98a1c507a3f414944c458d"));

        vSeeds.clear();

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
        cashaddrPrefix = "dimitest";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (        0, uint256S("0x0000d22b723620dca30ad399d9c5c145416cf1f3e34773f5ce6ffe3623264818")),
            1738340500,
                     1,
                3500.0
        };

        // A vector of p2sh addresses
        vDevFundAddress = { "" };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nMaxReorganizationDepth = 50;
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;
        consensus.powLimit = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimitV2 = uint256S("000000000000ffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nTargetTimespan = 16 * 60; // 16 mins
        consensus.nTargetSpacingV1 = 64;
        consensus.nTargetSpacing = 64;
        consensus.BIP34Height = -1; // BIP34 has not necessarily activated on regtest
        consensus.BIP34Hash = uint256();
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.fPoSNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108;// 75% for regtest
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        consensus.nProtocolV1RetargetingFixedTime = 1738340500;
        consensus.nProtocolV2Time = 1738340500;
        consensus.nProtocolV3Time = 1738340500;
        consensus.nProtocolV3_1Time = 1738340500;
        consensus.nLastPOWBlock = 1000;
        consensus.nStakeTimestampMask = 0xf;
        consensus.nCoinbaseMaturity = 10;

        pchMessageStart[0] = 0x70;
        pchMessageStart[1] = 0x35;
        pchMessageStart[2] = 0x22;
        pchMessageStart[3] = 0x06;
        nDefaultPort = 49555;
        nPruneAfterHeight = 100000;

        genesis = CreateGenesisBlock(1738340500, 34852, 0x1e0fffff, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();

        assert(consensus.hashGenesisBlock == uint256S("0x0000d22b723620dca30ad399d9c5c145416cf1f3e34773f5ce6ffe3623264818"));
        assert(genesis.hashMerkleRoot == uint256S("0x2a98e99882ffffbdec123030f141b1df946afe4f3a98a1c507a3f414944c458d"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        cashaddrPrefix = "dimireg";

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

    }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}

// Diminutivecoincoin: Donations to dev fund 
std::string CChainParams::GetDevFundAddress() const
{
    return !vDevFundAddress.empty() ? vDevFundAddress[0] : "";
}

CScript CChainParams::GetDevRewardScript() const
{
    CTxDestination dest = DecodeDestination(GetDevFundAddress());
    CScript scriptPubKey = GetScriptForDestination(dest);
    return scriptPubKey;
}
