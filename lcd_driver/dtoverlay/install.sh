#!/bin/bash

OVERLAYS="meson-g12a-i2c-ee-m3-gpioa-14-gpioa-15 meson-g12a-spi-lcd-4.0_drm_gpio meson-g12a-uart-ao-b-on-gpioao-8-gpioao-9"

sudo mkdir -p /boot/overlay-user
for overlay in $OVERLAYS; do
    sudo dtc -@ -I dts -O dtb $overlay.dts -o /boot/overlay-user/$overlay.dtbo
done

echo "user_overlays=$OVERLAYS" | sudo tee -a /boot/armbianEnv.txt > /dev/null
echo "extraargs=fbcon=map:22" | sudo tee -a /boot/armbianEnv.txt > /dev/null


