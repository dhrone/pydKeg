#!/bin/bash

sudo apt-get install libnss-mdns
git clone https://github.com/dhrone/pydKeg
cd pydKeg
sudo apt-get update
sudo apt-get install python-imaging
sudo apt-get install RPi.GPIO
sudo apt-get install python-pip
sudo pip install moment
sudo pip install pymysql
sudo apt-get install iputils-ping
