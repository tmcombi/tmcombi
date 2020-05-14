#!/bin/bash

sudo ./init_shared.sh

\cp ./data/nginx/conf.d.local/app.conf ./data/nginx/conf.d/app.conf

echo "### Starting nginx ..."
sudo docker-compose up --force-recreate -d nginx
echo

# nice link:
# https://devopscube.com/jenkins-master-build-slaves-docker-container/
