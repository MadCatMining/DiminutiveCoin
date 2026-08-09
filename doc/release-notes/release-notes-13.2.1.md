Diminutivecoin 13.2.1 Release Notes
===================================

Diminutivecoin version 13.2.1 is now available from:

  <https://github.com/MadCatMining/DiminutiveCoin/releases>

This is a bug-fix release. It repairs a fault that could wedge a mining node
on a stale block, corrects wallet balances during staking, and removes a
long-standing false network warning.

Upgrading is recommended for all users, and is particularly important for pool
operators and anyone running a mining node.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/MadCatMining/DiminutiveCoin/issues>


Compatibility
=============

**No flag day, and no coordinated upgrade is required.** 13.2.1, 13.2.0 and
13.1.x nodes validate every newly mined block identically, connect to each
other, and relay and stake together. You can upgrade whenever is convenient.

`PROTOCOL_VERSION` is unchanged at 70017 and `MIN_PEER_PROTO_VERSION` remains
70016, so no peer is disconnected for running an older version.

The wallet file format is unchanged, so downgrading is safe.


How to Upgrade
==============

Shut down the old version, wait until it has completely shut down, then run the
installer (on Windows) or replace `diminutivecoind` / `diminutivecoin-qt`
(on Linux and macOS).

**An existing data directory works as-is — no reindex is needed.**


Notable changes
===============

Fixed: mining node stuck retrying the same block
-------------------------------------------------

A node that mined or staked its own block could fail to disconnect it again if
that block later lost a reorganisation race, and would then retry the same
block indefinitely:

    ERROR: DisconnectBlock(): added transaction mismatch? database corrupted
    ERROR: DisconnectTip(): DisconnectBlock <hash> failed
    ERROR: ProcessNewBlock: ActivateBestChain failed

The node kept mining on a tip the rest of the network had already moved past,
so all of its work during that period was wasted. Only a restart cleared the
condition, after which the affected blocks were correctly recognised as orphans.

The cause was an inconsistency in how transaction timestamps were cached.
Transaction `nTime` is not carried through serialization for version 2
transactions and is read back as zero, but the chainstate cache was storing
whatever value a transaction held in memory. Blocks arriving from the network
or read from disk were unaffected, because their timestamps were already zero.
A block the node built itself was not: its own transactions carry a live
timestamp, and the node connects such a block directly from memory while it
always re-reads it from disk to disconnect it. The two copies disagreed, and
the disconnect was refused.

Nothing recovered from this on its own, because the cache is only cleaned up
after a connect or disconnect succeeds.

The chainstate cache now applies the same rule as the serializer, so an entry
always matches what a reload produces. This also closes a latent inconsistency
in input validation, where a mining node evaluated a timestamp rule against a
value its peers did not have.

Fixed: inflated balance and lingering orphaned rewards while staking
---------------------------------------------------------------------

During active staking the total balance grew beyond the coins actually held,
counting rewards from blocks that had been orphaned, and only returned to the
correct figure after a wallet restart.

Staked coins were being counted twice — once as available balance and once as
stake. When a coinstake was disconnected by a reorganisation the wallet dropped
its spend records, and when the same transaction was confirmed again those
records were not restored, so the wallet no longer knew its inputs had been
spent. A restart appeared to fix it because the spend index is rebuilt from
disk on load.

Transactions from orphaned blocks now also disappear from the transaction list
once the coins mature, rather than persisting until the next restart.

Fixed: spurious "the network does not appear to fully agree" warning
--------------------------------------------------------------------

Wallets have long shown this popup during normal operation:

    Warning: The network does not appear to fully agree!
    Some miners appear to be experiencing issues.

It was a false alarm. The check behind it is inherited from Bitcoin, where
every block carries roughly the same amount of work, so "a fork worth 7 blocks"
can be estimated as one block's work multiplied by seven. On a hybrid
proof-of-work / proof-of-stake chain that assumption does not hold — the two
block types run at independent difficulties, and a stake block can be worth
well over a thousand work blocks.

The result was that a single competing stake block, which is ordinary stake
competition and entirely harmless, could exceed the threshold by a factor of
several hundred and be reported as a large-work fork.

The check now measures a real span of the chain, which contains both block
types in their natural proportion, and additionally requires a fork to actually
be seven blocks long. Genuine large forks are still reported.


Known issues
============

* **"Unconfirmed" and "Immature" balances do not reflect staking.** Staking
  rewards appear in the "Stake" row instead. Coinstake transactions are never
  in the mempool, so they cannot be counted as unconfirmed, and the immature
  figure covers only proof-of-work rewards. This is a display limitation, not
  an accounting error — the total balance is correct.

* **Coinstake transaction IDs are not guaranteed unique.** Transaction `nTime`
  is not serialised for version 2 transactions, so it does not contribute to
  the transaction hash. Two staking attempts spending the same inputs with the
  same outputs therefore produce the same txid.

  The wallet accounting consequences of this are addressed in this release and
  in 13.2.0. At the chain level the duplicate-transaction rule (BIP30) is
  enforced on every block and cannot be triggered by this, because a coinstake
  spends its own input — a duplicate can only be mined on a chain where the
  original is absent, and there its outputs are not in the UTXO set. A full
  revalidation of the chain to height 1100000 recorded no occurrences.

* **The unit test suite does not build.** It was never adapted from the
  upstream Blackcoin More codebase. This does not affect the released binaries,
  which do not link the tests.


Credits
=======

Thanks to everyone who contributed to this release:

* MadCatMining
