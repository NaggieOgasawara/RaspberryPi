#!/bin/sh
sudo mount.vboxsf Share /home/naggie/share

rm /home/naggie/share/rpi-micon.img
cp rpi-micon.img /home/naggie/share/.
