# ops
Simple helper for devops scripts

This was inspired by [ansible](https://github.com/ansible/ansible) and [sps](https://github.com/wekan/sps)

Don't expect extensive maintenance on this project as it meets almost all my needs. An open source project is not :
* [free work](https://raccoon.onyxbits.de/blog/bugreport-free-support/)
* [open governance](https://words.werd.io/open-source-does-not-mean-open-governance-8ab751136106)
* [about you](https://gist.github.com/richhickey/1563cddea1002958f96e7ba9519972d9)

The main goal is to define commands (not the desired state) inside `*.ope.json` files and operate only selected commands like a Makefile.

## Requirements

* g++ (>= c++11)
* cmake (>= 3.0)

## Build

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
strip -s ops
```

## Prepare the host

* If you want to provision a remote host with ssh you should accept an ssh connexion with your gpg key :

```sh
ssh-keygen -t ed25519
ssh-copy-id -i ~/.ssh/id_ed25519.pub root@remote_host

# or

ssh-keygen -t rsa -b 4096
ssh-copy-id -i ~/.ssh/id_rsa.pub root@remote_host
```

* If you want to use rsync functionnalities rsync must be installed on local and remote

## Usage

```sh
ops
Operational Personal System

Use:
  ops [options] cmd [command] [json_files]

options:
  -a Address or hostname of the remote host. Must be provided with the ssh connection (by default).
  -b Base path for .ops.json files. Default is the current directory.
  -c Remote connection type. Default is 'ssh'. Legal values are 'ssh', 'chroot', 'local'
  -d Dry run. Do nothing.
  -h Display this help.
  -p Port of the remote connection.
  -q Quiet mode.
  -s Suffix of operations files. Default is '.ope.json'
  -u Remote user for the connection. Default is 'root'  -v Display version.
  -w Working directory. Default is the current directory.
  -x [tags] Exclude tags. Do not execute operation containing this tags.

command:
  Child node under the cmd node of the json file.
  - The slash separator '/' designates a hierarchy of childs.
  - The suffix ':all' designates all child of a hierarchy of childs.

    Example : 'ops -a remote_host cmd install' command designates the child node 'install' under the node 'cmd'

    # node.ope.json
    {
      "cmd":
      {
        "install": {}
      }
    }

    Example : 'ops -a remote_host cmd backup/data'

    # node.ope.json
    {
      "cmd":
      {
        "backup":
        {
          "data": {}
        }
      }
    }

    Example : 'ops -a remote_host cmd backup:all' designates all the children under the backup node

    # node.ope.json
    {
      "cmd":
      {
        "backup":
        {
          "conf": {}
        },
        "backup":
        {
          "data": {}
        }
      }
    }

```

## Few rules

* The definition file must ending with `.ope.json`
* `local_exec` : `bash` / `local`
* `remote_exec` : `bash` / `local` / `remote`
* `put` : `local` -> `remote`
* `get` : `remote` -> `local`
* `rsync_to_remote` : `local` -> `remote` (with `args`)
* `rsync_to_local` : `remote` -> `local` (with `args`)
* See the file [example.ope.json](./doc/example.ope.json)

## Execution

```sh
# Execute all install commands
ops -a remote_host cmd install

# Execute all backup with child conf commands
ops -a remote_host cmd backup/conf

# Execute all backup child commands
ops -a remote_host cmd backup:all
```
