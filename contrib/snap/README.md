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

A strictly confined snap gets its own `HOME`, so the data directory is

```
~/snap/diminutivecoin/current/.diminutivecoin
```

not `~/.diminutivecoin`. To carry an existing node across, stop everything
first and then move it:

```bash
mkdir -p ~/snap/diminutivecoin/current
mv ~/.diminutivecoin ~/snap/diminutivecoin/current/.diminutivecoin
```

Note that `~/snap/diminutivecoin/current` is a symlink to a revision
directory. Snapd copies the current revision's data forward on refresh, which
for a full chain is slow and doubles the disk use. A node with a large data
directory is better off keeping it outside the snap tree and pointing at it:

```bash
diminutivecoin.daemon -datadir=/path/to/chain
```

The `home` and `removable-media` interfaces are plugged for this. `home` is
connected automatically; `removable-media` is not:

```bash
sudo snap connect diminutivecoin:removable-media
```

Note that the `home` interface does not cover hidden directories in `$HOME` on
recent snapd versions, which is the other reason a relocated data directory
should not be a dotted path directly under `$HOME`.

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
