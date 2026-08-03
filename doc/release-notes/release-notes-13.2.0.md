Diminutivecoin 13.2.0 Release Notes
===================================

Diminutivecoin version 13.2.0 is now available from:

  <https://github.com/MadCatMining/DiminutiveCoin/releases>

This is a bug-fix release. It repairs syncing from scratch and reindexing, which
have been broken on mainnet since July 2025, and fixes wallet accounting errors
that cost staking nodes weight after chain reorganisations.

Upgrading is recommended for all users, and required for anyone who needs to
reindex, resync, or set up a new node.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/MadCatMining/DiminutiveCoin/issues>


Compatibility
=============

**No flag day, and no coordinated upgrade is required.** 13.2.0 and 13.1.x nodes
validate every newly mined block identically, connect to each other, and relay
and stake together. You can upgrade whenever is convenient.

`PROTOCOL_VERSION` is raised to 70017 so that 13.2.0 nodes are identifiable on
the network, but `MIN_PEER_PROTO_VERSION` is deliberately left at 70016 so that
no peer is disconnected for running the older version.

The wallet file format is unchanged, so downgrading back to 13.1.x is safe.


How to Upgrade
==============

Shut down the old version, wait until it has completely shut down, then run the
installer (on Windows) or replace `diminutivecoind` / `diminutivecoin-qt`
(on Linux and macOS).

**An existing data directory works as-is — no reindex is needed.** If you
previously attempted a reindex or a fresh sync with 13.1.x and it stopped with
`ConnectBlock(): incorrect difficulty`, delete the partial chainstate and start
again with 13.2.0; it will now complete.


Notable changes
===============

Fixed: sync from scratch and reindex fail with "incorrect difficulty"
---------------------------------------------------------------------

Any node syncing from zero, or reindexing an existing data directory, stopped at
block 2596 with:

    ERROR: ConnectBlock(): incorrect difficulty
    InvalidChainFound: invalid block=f6fb3623...  height=2596

and at further heights beyond it.

The cause was a consensus constant that was replaced rather than versioned. In
July 2025 the proof-of-stake minimum-difficulty limit was reduced from
`000000000000ffff...` to `00000fffffffffff...`. Once the original value was gone
from the source, the client could no longer reproduce the difficulty of the many
early blocks that had been clamped to it, when stake was thin and the retarget
kept bottoming out.

Running nodes were unaffected, because block difficulty is only recomputed when a
block is first connected. That is why the problem stayed invisible for a year and
appeared only on reindex or a fresh sync.

13.2.0 keeps both limits and selects between them by height. The switch point,
block **190927** (2025-07-14), was measured against the live chain rather than
guessed, and both it and the surrounding behaviour are documented in
`doc/pos-limit-reduction.md`.

Note that checkpoints could not have worked around this. Checkpoints only
disable script checks for blocks below the last checkpoint; block difficulty is
a consensus rule and is always verified.

Fixed: stale wallet accounting after proof-of-stake reorganisations
-------------------------------------------------------------------

When a locally generated coinstake was disconnected by a reorganisation, the
wallet failed to fully clean up its internal spend tracking. This had three
effects, in increasing order of severity:

* **Incorrect balances.** Balance and stake figures could remain wrong until the
  wallet was restarted or rescanned.

* **Lost staking weight.** Inputs of the orphaned coinstake stayed marked as
  spent, so those coins silently dropped out of the staking set and stopped
  earning until the next restart. On a node that reorganises occasionally this
  quietly eroded staking performance.

* **Possible crashes.** Two assertion failures were reachable when a coinstake
  was not itself present in the wallet, which could abort the process.

Additionally, transaction abandonment stopped one level deep instead of
recursing through descendant spends, and empty placeholder entries could be
inserted into the wallet.

Thanks to **dtd-tosh** for finding and fixing this.

Fixed: spurious "the network does not appear to fully agree" warning
--------------------------------------------------------------------

Wallets have long shown this popup during normal operation:

    Warning: The network does not appear to fully agree!
    Some miners appear to be experiencing issues.

It was a false alarm. The check behind it is inherited from Bitcoin, where every
block carries roughly the same amount of work, so "a fork worth 7 blocks" can be
estimated as one block's work multiplied by seven. On a hybrid proof-of-work /
proof-of-stake chain that assumption does not hold — the two block types run at
independent difficulties, and a stake block can be worth well over a thousand
work blocks.

The result was that a single competing stake block, which is ordinary stake
competition and entirely harmless, could exceed the threshold by a factor of
several hundred and be reported as a large-work fork.

The check now measures a real span of the chain, which contains both block types
in their natural proportion, and additionally requires a fork to actually be
seven blocks long. Genuine large forks are still reported.

Updated checkpoints and minimum chain work
-------------------------------------------

* New checkpoints at block **1100000** and at block **190927**, the first block
  using the reduced proof-of-stake limit.
* `nMinimumChainWork` advanced from block 43000 to block 1100000, raising the
  work a candidate chain must demonstrate during sync by roughly 19x.
* Sync progress figures corrected. The estimated transaction rate had been
  understating actual throughput by about three times, which made the progress
  percentage misleading during the final stage of a sync.

New `-scanposlimit` diagnostic option
--------------------------------------

A debugging option used to locate difficulty divergences, which logs them
instead of rejecting the block.

**This option disables a consensus rule.** It exists to produce a measurement on
a throwaway data directory and must never be used on a wallet or a peer-facing
node. See `doc/pos-limit-reduction.md`.


Known issues
============

* **Coinstake transaction IDs are not guaranteed unique.** Transaction `nTime` is
  not serialised for version 2 transactions, so it does not contribute to the
  transaction hash. Two staking attempts spending the same inputs with the same
  outputs therefore produce the same txid.

  This is a quirk rather than an active fault. Its practical consequence was the
  wallet accounting problem fixed in this release: the wallet indexes
  transactions by txid, so an orphaned coinstake and a later re-mined one shared
  a single entry. At the chain level the duplicate-transaction rule (BIP30) is
  enforced on every block and cannot be triggered by this, because a coinstake
  spends its own input — a duplicate can only be mined on a chain where the
  original is absent, and there its outputs are not in the UTXO set. A full
  revalidation of the chain to height 1100000 recorded no occurrences.

  Making coinstake txids unique would mean changing the transactions the wallet
  produces, which is not justified by any observed failure. If you do encounter
  duplicate-transaction or overwrite errors, please open an issue with the exact
  log text.

* **The unit test suite does not build.** It was never adapted from the upstream
  Blackcoin More codebase. This does not affect the released binaries, which do
  not link the tests.


Credits
=======

Thanks to everyone who contributed to this release:

* MadCatMining
* dtd-tosh
