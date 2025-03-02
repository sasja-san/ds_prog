
# DSProg

This is a minimal example, originally by ndsdev forum user nin-kuuku:
<https://forums.nesdev.org/viewtopic.php?t=22936>

The work by nin-kuuku (`DSProg.zip`) contains the needed programs for Windows.
Here, instead, are instructions on how to set it up in Arch Linux.



# Resource Directory

Here the binaries (`fasmarm` and `ndstool`) needed to make the rom are
included.
Everything *should* just work by running `make rom`.
The remainder of this document contains instructions for setting up the tools 
needed, on an Arch system[^manjaro].

[^manjaro]: Only tested on Manjaro. It's close enough.



# FASMARM

FASMARM is a version of FASM (flat assembler) which cross-compiles ARM assembly 
on an x86 machine: <https://arm.flatassembler.net>

Somewhere in your `$PATH` (e.g. `~/.local/bin/`) run this:

```
wget -qO- https://arm.flatassembler.net/FASMARM_full.ZIP | bsdtar - -x fasmarm && chmod u+x fasmarm
```


# NdsTool (DevkitPro)

Some of these instructions are for the `fish` shell (specifically the 
`set` commands). Adjust if needed. This is more or less ripped directly from 
<https://devkitpro.org/wiki/devkitPro_pacman>.

Environment vars:

```
set -U DEVKITPRO /opt/devkitpro
set -U DEVKITARM /opt/devkitpro/devkitARM
set -U DEVKITPPC /opt/devkitpro/devkitPPC
```

Path update:

```
set -U fish_user_paths /opt/devkitpro/devkitARM/bin $fish_user_paths
set -U fish_user_paths /opt/devkitpro/tools/bin     $fish_user_paths
```

Pacman modifying:

```
sudo pacman-key --recv BC26F752D25B92CE272E0F44F7FD5492264BB9D0 --keyserver keyserver.ubuntu.com
sudo pacman-key --lsign BC26F752D25B92CE272E0F44F7FD5492264BB9D0
sudo pacman -U https://pkg.devkitpro.org/devkitpro-keyring.pkg.tar.zst
sudo pacman-key --populate devkitpro
```

Edit /etc/pacman.conf to add these lines:

```
[dkp-libs]
Server = https://pkg.devkitpro.org/packages

[dkp-linux]
Server = https://pkg.devkitpro.org/packages/linux/$arch/
```

Now update and install. Last command is optional:

```
sudo pacman -Syu
sudo pacman -Sl
sudo pacman -S ndstool
sudo pacman -S nds-dev
```





