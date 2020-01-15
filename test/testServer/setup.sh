#!/bin/bash
# file: setup.sh
# setup minimal ubuntu installation as a test server
apt-get update && apt-get upgrade -y
docker pull jenkins
docker run -d -v jenkins_home:/var/jenkins_home -p 8080:8080 -p 50000:50000 \
  --restart unless-stopped --env 'JAVA_OPTS=-Dhudson.model.DirectoryBrowserSupport.CSP="sandbox allow-scripts;"' jenkins/jenkins
