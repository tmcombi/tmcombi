#!/bin/bash

sudo ./init_shared.sh

if ! [ -x "$(command -v ifconfig)" ]; then
    apt update && apt upgrade -y && apt install -y net-tools
fi

\cp ./data/nginx/conf.d.local/app.conf ./data/nginx/conf.d/app.conf

echo "### Starting nginx ..."
sudo docker-compose up --force-recreate -d nginx
echo
