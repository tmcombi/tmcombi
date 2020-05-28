#!/bin/bash

if ! [ -x "$(command -v docker)" ]; then
    apt update && apt upgrade -y && apt install -y docker.io
fi
if ! [ -x "$(command -v docker-compose)" ]; then
    apt update && apt upgrade -y && apt install -y docker-compose
fi

sudo useradd jenkins || true
sudo usermod -aG docker jenkins
sudo mkdir -p /tmp/build_output/
sudo chown jenkins:jenkins /tmp/build_output

echo "### Starting jenkins ..."
sudo docker-compose up --force-recreate -d jenkins
echo
echo "### Starting portainer ..."
sudo docker-compose up --force-recreate -d portainer
echo
echo "### Starting registry ..."
sudo docker-compose up --force-recreate -d registry
echo
