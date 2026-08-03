// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"
#include <stdio.h>

/**
 * The minimum-difficulty limit for a block at height nHeight with timestamp nTime.
 *
 * nHeight must be the height of the block whose target is being computed, not the
 * height of whatever index the caller happens to be walking back from: the PoS limit
 * changes at a fixed height, and the two call sites below reference different blocks
 * (the chain tip vs. the last block of the same type), which would otherwise pick
 * different limits for blocks straddling the switch.
 */
static arith_uint256 GetTargetLimit(int nHeight, int64_t nTime, const Consensus::Params& params, bool fProofOfStake)
{
    uint256 nLimit;

    if (fProofOfStake) {
        if (nHeight >= params.nPosLimitV2ReducedHeight)
            nLimit = params.posLimitV2Reduced;
        else if (params.IsProtocolV2(nTime))
            nLimit = params.posLimitV2;
        else
            nLimit = params.posLimit;
    } else {
        nLimit = params.powLimit;
    }

    return UintToArith256(nLimit);
}

unsigned int GetNextTargetRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params, bool fProofOfStake)
{
    // Genesis block
    if (pindexLast == NULL)
        return UintToArith256(params.powLimit).GetCompact();

    const int nHeight = pindexLast->nHeight + 1;
    const CBlockIndex* pindexPrev = GetLastBlockIndex(pindexLast, fProofOfStake);
    unsigned int nTargetLimit = GetTargetLimit(nHeight, pindexLast->GetBlockTime(), params, fProofOfStake).GetCompact();

    if (pindexPrev->pprev == NULL)
        return nTargetLimit; // first block
    const CBlockIndex* pindexPrevPrev = GetLastBlockIndex(pindexPrev->pprev, fProofOfStake);
    if (pindexPrevPrev->pprev == NULL)
        return nTargetLimit; // second block

    return CalculateNextTargetRequired(pindexPrev, pindexPrevPrev->GetBlockTime(), nHeight, params, fProofOfStake);
}

unsigned int CalculateNextTargetRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, int nHeight, const Consensus::Params& params, bool fProofOfStake)
{
    if (fProofOfStake) {
        if (params.fPoSNoRetargeting)
            return pindexLast->nBits;
    } else {
        if (params.fPowNoRetargeting)
            return pindexLast->nBits;
    }

    int64_t nTargetSpacing = params.IsProtocolV2(pindexLast->GetBlockTime()) ? params.nTargetSpacing : params.nTargetSpacingV1;
    int64_t nActualSpacing = pindexLast->GetBlockTime() - nFirstBlockTime;

    // Limit adjustment step
    if (params.IsProtocolV1RetargetingFixed(pindexLast->GetBlockTime()) && nActualSpacing < 0)
        nActualSpacing = nTargetSpacing;
    if (params.IsProtocolV3(pindexLast->GetBlockTime()) && nActualSpacing > nTargetSpacing*10)
        nActualSpacing = nTargetSpacing*10;

    // retarget with exponential moving toward target spacing
    const arith_uint256 bnTargetLimit = GetTargetLimit(nHeight, pindexLast->GetBlockTime(), params, fProofOfStake);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    int64_t nInterval = params.nTargetTimespan / nTargetSpacing;
    bnNew *= ((nInterval - 1) * nTargetSpacing + nActualSpacing + nActualSpacing);
    bnNew /= ((nInterval + 1) * nTargetSpacing);

    if (bnNew <= 0 || bnNew > bnTargetLimit)
        bnNew = bnTargetLimit;

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
