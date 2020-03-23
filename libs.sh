#!/bin/sh
try() {
	if [ "which $2 > /dev/null" ]; then
		use_$1 $2
		exit
	fi
}

use_deb(){
	$1 install \
		pkg-config \
		gcc \
		make \
		libvulkan-dev \
		libsdl2-dev \
		vulkan-validationlayers \
		glslang-tools
}

try deb apt-get
try deb apt
