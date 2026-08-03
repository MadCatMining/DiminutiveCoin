# The PoS minimum-difficulty limit reduction

## Background

Commit `e40a655` ("Reduced POS difficulty", 2025-07-13) changed the mainnet
proof-of-stake minimum-difficulty limit:

| | value | compact `nBits` | log2(work) |
|---|---|---|---|
| original | `000000000000ffff…` | `0x1b00ffff` | 48.0 |
| reduced  | `00000fffffffffff…` | `0x1e0fffff` | 20.0 |

Because the change replaced the constant outright, the released code could no
longer reproduce the difficulty of any historical block that had been clamped to
the *original* limit. Those blocks are common early in the chain, when there was
little stake and the retarget kept bottoming out at the limit — e.g. mainnet
block 2596, whose `log2_work=48.000344` is the original limit almost exactly.

A node that already had those blocks in its index kept running fine, since
`ConnectBlock` only recomputes `nBits` when it connects a block. But any node
syncing from scratch, or reindexing, re-validates them and fails with:

```
ERROR: ConnectBlock(): incorrect difficulty
```

Checkpoints do not help here, and cannot be made to: `Checkpoints::GetLastCheckpoint`
is only used to skip *script* checks (`main.cpp`) and to reject forks below the
last checkpoint. The difficulty check is a consensus rule that runs unconditionally.
Bitcoin's `-assumevalid` has the same limitation for the same reason.

The fix is to keep both constants and select between them by height:

* `consensus.posLimitV2` — original limit, used below `nPosLimitV2ReducedHeight`
* `consensus.posLimitV2Reduced` — reduced limit, used at and above it

## Determining `nPosLimitV2ReducedHeight`

The switch height cannot be derived from the source, only from the chain. With
`nPosLimitV2ReducedHeight` left at `INT_MAX` (the original limit applies
everywhere), every block that was mined under the *reduced* limit and clamped to
it will fail the difficulty check. The lowest such height is the switch height.

1. Build with the current values (`nPosLimitV2ReducedHeight = INT_MAX`).

2. Sync or reindex with the diagnostic flag, which logs divergences instead of
   rejecting the block:

   ```
   diminutivecoind -reindex -scanposlimit -debug=1
   ```

   **`-scanposlimit` disables a consensus rule.** Use it only to produce this
   measurement, on a node whose chain you are willing to discard afterwards.
   Never run a wallet or a peer-facing node with it.

3. Collect the results:

   ```
   grep POSLIMIT-SCAN ~/.diminutivecoin/debug.log | head
   ```

   Each line reports `height`, the block's stored `nBits`, and the `expected`
   value. Every line should be `pos=1` with `expected=1b00ffff` — the original
   limit being wrongly applied — and a stored `nBits` easier than `1b00ffff`.
   A `pos=0` line, or an `expected` other than `1b00ffff`, is a different bug
   and not covered by this fix.

4. Set `consensus.nPosLimitV2ReducedHeight` in `CMainParams` to the **lowest**
   height from that list.

   For mainnet this measurement gave **190927** (block
   `7e907a0d1d47bbfecd30e34edf77f545b0d2334156dbdaf8748c300ed63ebf09`, stored
   `nBits=1b021ffd`). Note that the post-switch blocks are not clamped to the
   reduced limit — they are free-running targets that merely sit *below* the
   original limit, which is what the original limit had been suppressing.

5. Rebuild **without** the diagnostic and reindex again:

   ```
   diminutivecoind -reindex
   ```

   A clean sync to tip means every historical block now validates under the
   correct-era limit. If a `POSLIMIT-SCAN`-free run still reports
   `incorrect difficulty`, the error message now includes the height and both
   `nBits` values, which identifies the block directly.

## Shortcut if you have a working synced node

The switch height is bounded by the last block using the original limit and the
first block using the reduced one, so a synced node can find it over RPC without
the diagnostic build — scan `getblockheader` in the region around 2025-07-13 for
the transition from `bits: "1b00ffff"` to `bits: "1e0fffff"`.

## Network compatibility

This is **not** a fork of the rules applied to new blocks, and no coordinated
upgrade is required.

Pre-13.2.0 code applies the reduced limit at every height. 13.2.0 applies it at
every height at or above `nPosLimitV2ReducedHeight` (190927). The chain passed
that height in July 2025, so for every block mined from then on the two versions
compute **identical** difficulty and interoperate normally — staking, relay and
mining are unaffected.

The versions differ only when re-validating history *below* 190927, which
happens during a reindex or a sync from scratch. A 13.1.x node keeps following
the chain it already has; it just cannot rebuild its index.

`MIN_PEER_PROTO_VERSION` is therefore deliberately left at 70016 while
`PROTOCOL_VERSION` advertises 70017. Raising the minimum would disconnect peers
that are fully compatible, partitioning the network for no consensus benefit.
Raise it only when new blocks are actually validated differently.

## Other networks

Testnet and regtest never received the reduction; they set
`posLimitV2Reduced = posLimitV2` and `nPosLimitV2ReducedHeight = INT_MAX`, which
is behaviourally identical to the previous code.
