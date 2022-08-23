#!/bin/sh
profile_moonflower() {
	profile_standard
	apks="$apks nano"

	# For X
	apks="$apks xorg-server xf86-input-libinput eudev eudev-openrc mesa"
	apks="$apks dbus dbus-openrc xfce4 xfce4-terminal"

	# For installer
	apks="$apks openssh grub-bios grub virtualbox-guest-additions"
	apks="$apks udev-init-scripts udev-init-scripts-openrc"

	apkovl="genapkovl-moonflower.sh"
}
