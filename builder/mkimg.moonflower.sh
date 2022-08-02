#!/bin/sh
profile_moonflower() {
	profile_standard
	apks="$apks nano"
	apks="$apks xorg-server xf86-input-libinput eudev eudev-openrc mesa"
	apks="$apks dbus dbus-openrc xfce4 xfce4-terminal"

	apkovl="genapkovl-moonflower.sh"
}
