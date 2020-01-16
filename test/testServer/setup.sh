#!/bin/bash
# file: setup.sh
# setup minimal ubuntu installation as a test server

## update & docker
apt update && apt upgrade -y
apt install -y docker.io


## jenkins
docker pull jenkins/jenkins
docker run -d -v jenkins_home:/var/jenkins_home -p 8080:8080 -p 50000:50000 --name jenkins --restart unless-stopped --env 'JAVA_OPTS=-Dhudson.model.DirectoryBrowserSupport.CSP="sandbox allow-scripts;"' jenkins/jenkins
echo "Waiting for initial jenkins-admin password..."
until [ -f /var/lib/docker/volumes/jenkins_home/_data/secrets/initialAdminPassword ]
do
     sleep 1
done
echo "initial jenkins-admin passowrd:"
sleep 1
cat /var/lib/docker/volumes/jenkins_home/_data/secrets/initialAdminPassword
sleep 1

## portainer
docker pull portainer/portainer
docker run -d -v portainer_data:/data -p 9000:9000 -p 8000:8000 --name portainer --restart unless-stopped -v /var/run/docker.sock:/var/run/docker.sock portainer/portainer

