#!/bin/bash
# file: setup.sh
# setup minimal ubuntu installation as a test server

## update & docker
apt update && apt upgrade -y
apt install -y docker.io

## nginx
docker pull nginx
docker run -d -v nginx_html:/usr/share/nginx/html:ro -v nginx_conf:/etc/nginx:ro -p 80:80 -p 443:443 --restart unless-stopped nginx

## jenkins
docker pull jenkins/jenkins:lts
docker run -d -v jenkins_home:/var/jenkins_home -p 8080:8080 -p 50000:50000 --restart unless-stopped --env JENKINS_OPTS="--prefix=/jenkins" --env 'JAVA_OPTS=-Dhudson.model.DirectoryBrowserSupport.CSP="sandbox allow-scripts;"' jenkins/jenkins:lts
echo "Waiting for initial jenkins-admin password..."
until [ -f /var/lib/docker/volumes/jenkins_home/_data/secrets/initialAdminPassword ]
do
     sleep 1
done
echo "initial jenkins-admin password:"
sleep 1
cat /var/lib/docker/volumes/jenkins_home/_data/secrets/initialAdminPassword
sleep 1

## portainer
docker pull portainer/portainer:lts
docker run -d -v portainer_data:/data -p 9000:9000 -p 8000:8000 --restart unless-stopped -v /var/run/docker.sock:/var/run/docker.sock portainer/portainer:lts

