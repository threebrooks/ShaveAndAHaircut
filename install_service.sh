#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo "[Unit]
Description=Shave and a haircut
After=network-online.target

[Service]
ExecStart=/bin/bash $DIR/run.sh
WorkingDirectory=$DIR
StandardOutput=inherit
StandardError=inherit
Restart=always
User=pi

[Install]
WantedBy=multi-user.target" > /lib/systemd/system/ShaveAndAHaircut.service 

sudo systemctl enable ShaveAndAHaircut.service
sudo systemctl start ShaveAndAHaircut.service

