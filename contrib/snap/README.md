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

The snap uses the standard `~/.diminutivecoin`, so an existing node is picked
up as-is and nothing has to be moved.

That is not what a snap does by default, and it takes two pieces to arrange.

**Confinement gives the snap its own `HOME`.** A strictly confined app runs
with `HOME=$SNAP_USER_DATA`, so the built-in default would resolve to
`~/snap/diminutivecoin/current/.diminutivecoin`. `bin/diminutivecoin-launcher`
therefore passes `-datadir` explicitly, pointing at `$SNAP_REAL_HOME`. It does
*not* reassign `HOME`, which would drag Qt's own config and cache out of the
snap along with it. An explicit `-datadir` on the command line always wins.

**The `home` interface does not cover dot-directories.** Its AppArmor profile
grants `@{HOME}/[^.]**`, so `~/.diminutivecoin` is excluded no matter what
path is passed. Access comes from a `personal-files` plug instead, which snapd
does not connect automatically:

```bash
sudo snap connect diminutivecoin:dot-diminutivecoin
```

Until that is connected every access is denied, so the launcher tests the
directory first and falls back to the snap's private one with an explanatory
message rather than failing to start. If the wallet looks empty on first run,
this is why.

Auto-connection requires a snap declaration granted by the Snap Store — see
*Publishing* below.

### The daemon service

`diminutivecoin.daemon` runs as root, so its `$HOME` is `/root` and its data
directory is `/root/.diminutivecoin`. The `personal-files` rule covers this,
since it is written against `$HOME` rather than a fixed path.

This is deliberately not the desktop user's directory: two nodes sharing one
data directory would corrupt it, and the service has no way to know which user
owns the wallet. To point the service somewhere else, put a `datadir=` line in
`/root/.diminutivecoin/diminutivecoin.conf`, or run the foreground command
`diminutivecoin.diminutivecoind -datadir=...` instead of the service.

### Keeping the chain elsewhere

Any path the `home` or `removable-media` interfaces reach works:

```bash
diminutivecoin.diminutivecoind -datadir=/path/to/chain
```

`home` is connected automatically; `removable-media` is not:

```bash
sudo snap connect diminutivecoin:removable-media
```

Note that these interfaces still exclude dot-directories, so an explicit
`-datadir` should not be a hidden path directly under `$HOME`.

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

### Auto-connecting `dot-diminutivecoin`

`personal-files` is never auto-connected on installation without a snap
declaration, which is granted per-snap by the store reviewers. Until one is in
place every user has to run `snap connect` by hand after installing.

To request it, open a thread under *store-requests* on
<https://forum.snapcraft.io/> naming the snap and the interface, and explain
why the path is needed — here, that it is the established data directory of an
existing wallet and users upgrading from a tarball or `.deb` install already
have a populated `~/.diminutivecoin`. Requests for a snap's own conventional
dot-directory are routine.
