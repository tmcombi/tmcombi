#!/bin/bash
# file: setup.sh
# setup minimal ubuntu installation as a test server
apt update && apt upgrade -y
apt install -y docker.io
docker pull jenkins/jenkins
#docker volume create jenkins_home
docker run -d -v jenkins_home:/var/jenkins_home -p 8080:8080 -p 50000:50000 \
  --restart unless-stopped --env 'JAVA_OPTS=-Dhudson.model.DirectoryBrowserSupport.CSP="sandbox allow-scripts;"' jenkins/jenkins
echo "Waiting for initial jenkins-admin password..."
until [ -f /var/lib/docker/volumes/jenkins_home/_data/secrets/initialAdminPassword ]
do
     sleep 1
done
echo "initial jenkins-admin passowrd:"
sleep 1
cat /var/lib/docker/volumes/jenkins_home/_data/secrets/initialAdminPassword
sleep 1
