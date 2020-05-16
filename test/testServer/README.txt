1. run ./init_local.sh or ./init_cloud.sh

2. to deactivate containers use:
      docker-compose down
      
3. to reactivate the containers
      locally: ./init_local.sh
      in cloud: doocker-compose up -d

4. get the temporary jenkins password
       cat /var/lib/docker/volumes/testserver_jenkins_home/_data/secrets/initialAdminPassword

5. go to
      http://localhost/jenkins
   or
      http://www.trainamon.com/jenkins
   end set up jenkins

5. go to
      http://localhost/portainer
   or
      http://www.trainamon.com/portainer
   and set up portainer (especially new password)
