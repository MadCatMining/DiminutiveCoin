# Snap packaging

`snap/snapcraft.yaml` at the top of the tree builds a strictly confined snap
containing the daemon, the Qt wallet and the command line tools.

## Building

```bash
sudo snap install snapcraft --classic
sudo snap install lxd        # if not already present
sudo lxd init --auto
snapcraft
```

`snapcraft` builds in an LXD container, so the host does not need any of the
build dependencies. The result is a `.snap` file in the working directory:

```bash
sudo snap install --dangerous ./diminutivecoin_13.2.1_amd64.snap
```

`--dangerous` is required for a locally built snap because it is not signed by
the store.

To iterate on a failed build, `snapcraft --debug` drops you into a shell inside
the container at the point of failure.

## What gets installed

| command | binary |
|---|---|
| `diminutivecoin.qt` | `diminutivecoin-qt` (also appears in the applications menu) |
| `diminutivecoin.daemon` | `diminutivecoind` run as a systemd service |
| `diminutivecoin.diminutivecoind` | `diminutivecoind` run in the foreground |
| `diminutivecoin.cli` | `diminutivecoin-cli` |
| `diminutivecoin.tx` | `diminutivecoin-tx` |

The daemon service is installed **disabled**, so that installing the snap does
not silently start syncing the chain. Enable it with:

```bash
sudo snap start --enable diminutivecoin.daemon
sudo snap logs -f diminutivecoin.daemon
```

## Data directory

A strictly confined snap runs with `HOME` set to its own directory, so the
data directory is

```
~/snap/diminutivecoin/current/.diminutivecoin
```

not `~/.diminutivecoin`. **The wallet file to back up is `wallet.dat` inside
that folder.**

This is left at the snap default on purpose. Pointing the snap at the usual
`~/.diminutivecoin` is possible but not free: the `home` interface grants
`@{HOME}/[^.]**`, so hidden directories in the real home are excluded from it,
and reaching one needs a `personal-files` plug that snapd will not connect
without a store declaration. That means either a manual `snap connect` step on
every install or a forum request. Not worth it for a package with a handful of
downloads a month.

The consequence is that the snap is a **separate node** from an existing
tarball install — it syncs its own copy of the chain and does not see an
existing wallet. Anyone with a node already running is better served by the
release binaries.

To move an existing node in anyway, stop everything first:

```bash
mv ~/.diminutivecoin ~/snap/diminutivecoin/current/.diminutivecoin
```

Note that `~/snap/diminutivecoin/current` is a symlink to a revision
directory, and snapd copies the current revision's data forward on every
refresh — for a full chain that is slow and briefly doubles the disk use.

To keep the chain somewhere else entirely:

```bash
diminutivecoin.diminutivecoind -datadir=/path/to/chain
```

Any path reachable through `home` or `removable-media` works, as long as it is
not a hidden directory. `home` is connected automatically; `removable-media`
is not:

```bash
sudo snap connect diminutivecoin:removable-media
```

## Architectures

`amd64` and `arm64` are built. Snapcraft builds each one natively, so a local
`snapcraft` run produces a snap for whatever machine you run it on; the store's
build service covers both when the source is attached to it.

Nothing in the source is x86-specific. The x86 intrinsic paths all guard
themselves:

* `crypto/sha256_sse4.cpp` is wrapped in `#if defined(__x86_64__)`, and nothing
  references `sha256_sse4::Transform` in any case.
* `crypto/scrypt-sse2.cpp` is wrapped in `#if defined(USE_SSE2)`, and
  `--enable-sse2` is off by default.
* The SSE4.1, AVX2 and SHA-NI objects are gated by `AX_CHECK_COMPILE_FLAG`
  probes in `configure.ac`, which fail on non-x86 and leave those parts out.

`configure.ac` also carries an ARM CRC path (`-march=armv8-a+crc+crypto`,
`arm_acle.h` / `arm_neon.h`), so aarch64 is a supported target upstream rather
than an accident.

`armhf`, `ppc64el`, `s390x` and `riscv64` are all plausible and none are
listed, because none have been built and run. Add a `build-on` entry once one
has been.

## Berkeley DB

The wallet is built against Berkeley DB 6.2.38, built from source as its own
part. Ubuntu packages BDB 5.3, and a wallet written by a build linked against
it would not be portable to the released binaries. The version and hash in
`snap/snapcraft.yaml` are kept in step with `depends/packages/bdb.mk`.

## Qt

The GUI app uses the `kde-neon` extension, which supplies Qt 5.15 from a
content snap rather than bundling it. This keeps the snap small and gives the
wallet the host's theme and fonts.

## Publishing

```bash
snapcraft login
snapcraft upload --release=stable diminutivecoin_13.2.1_amd64.snap
```

The version is taken from `git describe --tags`, falling back to the version
macros in `configure.ac` when the tree has no tags — so build from a tagged
checkout for a release upload.
