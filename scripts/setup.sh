#/bin/sh

printf "o\nn\np\n1\n2048\n+1g\na\n1\nw\n" | fdisk /dev/sda
mkfs.ext4 /dev/sda1
mount -t ext4 /dev/sda1 /mnt
#export BOOTLOADER=grub
setup-alpine -f scripts/setup.txt
