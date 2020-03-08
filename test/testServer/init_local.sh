#!/bin/bash

sudo ./init_shared.sh

echo "### Starting nginx ..."
sudo docker-compose up --force-recreate -d nginx
echo
