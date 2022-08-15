#!/bin/sh
profile_installerdebug() {
	profile_standard
	apks="$apks nano openssh grub-bios grub sshfs virtualbox-guest-additions"

	apks="$apks eudev eudev-openrc udev-init-scripts udev-init-scripts-openrc"

	apkovl="genapkovl-installerdebug.sh"
}
