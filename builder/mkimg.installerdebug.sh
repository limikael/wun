#!/bin/sh
profile_installerdebug() {
	profile_standard
	apks="$apks nano openssh grub-bios grub sshfs virtualbox-guest-additions"

	apkovl="genapkovl-installerdebug.sh"
}
