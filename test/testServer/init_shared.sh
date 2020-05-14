#!/bin/bash

if ! [ -x "$(command -v docker)" ]; then
    apt update && apt upgrade -y && apt install -y docker.io
fi
if ! [ -x "$(command -v docker-compose)" ]; then
    apt update && apt upgrade -y && apt install -y docker-compose
fi

echo "### Starting jenkins ..."
sudo docker-compose up --force-recreate -d jenkins
echo "Waiting for initial jenkins-admin password..."
until [ -f /var/lib/docker/volumes/testserver_jenkins_home/_data/secrets/initialAdminPassword ]
do
     sleep 1
done
echo "initial jenkins-admin password:"
cat /var/lib/docker/volumes/testserver_jenkins_home/_data/secrets/initialAdminPassword
sleep 1
echo
echo "### Starting portainer ..."
sudo docker-compose up --force-recreate -d portainer
echo
echo "### Starting registry ..."
sudo docker-compose up --force-recreate -d registry
echo
