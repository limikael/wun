#!/bin/sh -e

HOSTNAME="$1"
if [ -z "$HOSTNAME" ]; then
	echo "usage: $0 hostname"
	exit 1
fi

cleanup() {
	rm -rf "$tmp"
}

makefile() {
	OWNER="$1"
	PERMS="$2"
	FILENAME="$3"
	cat > "$FILENAME"
	chown "$OWNER" "$FILENAME"
	chmod "$PERMS" "$FILENAME"
}

rc_add() {
	mkdir -p "$tmp"/etc/runlevels/"$2"
	ln -sf /etc/init.d/"$1" "$tmp"/etc/runlevels/"$2"/"$1"
}

tmp="$(mktemp -d)"
trap cleanup EXIT

mkdir -p "$tmp"/etc/local.d
makefile root:root 0755 "$tmp"/etc/local.d/live.start <<EOF
echo "Setting up live environment..."
startx
EOF

mkdir -p "$tmp"/etc/apk
makefile root:root 0644 "$tmp"/etc/apk/world <<EOF
alpine-base
eudev
eudev-openrc
mesa
openssl
udev-init-scripts
udev-init-scripts-openrc
xf86-input-libinput
xorg-server
xfce4
xfce4-terminal
EOF

rc_add devfs sysinit
rc_add dmesg sysinit
rc_add udev sysinit
rc_add udev-settle sysinit
rc_add udev-trigger sysinit
rc_add hwdrivers sysinit
rc_add modloop sysinit

rc_add hwclock boot
rc_add modules boot
rc_add sysctl boot
rc_add hostname boot
rc_add bootmisc boot
rc_add syslog boot

rc_add mount-ro shutdown
rc_add killprocs shutdown
rc_add savecache shutdown

rc_add local default
rc_add udev-postmount default
rc_add dbus default

tar -c -C "$tmp" etc | gzip -9n > $HOSTNAME.apkovl.tar.gz
