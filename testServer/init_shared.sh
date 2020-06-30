#!/bin/bash

if ! [ -x "$(command -v docker)" ]; then
    apt update && apt upgrade -y && apt install -y docker.io
fi
if ! [ -x "$(command -v docker-compose)" ]; then
    apt update && apt upgrade -y && apt install -y docker-compose
fi

sudo useradd -m jenkins || true
sudo usermod -aG docker jenkins

echo "### Starting jenkins ..."
sudo docker-compose up --force-recreate -d jenkins
echo
echo "### Starting portainer ..."
sudo docker-compose up --force-recreate -d portainer
echo
echo "### Starting registry ..."
sudo docker-compose up --force-recreate -d registry
echo
echo "### Starting db ..."
sudo docker-compose up --force-recreate -d db
echo
echo "### Starting wordpress ..."
sudo docker-compose up --force-recreate -d wordpress
echo
echo "### Starting phpmyadmin ..."
sudo docker-compose up --force-recreate -d phpmyadmin
echo
