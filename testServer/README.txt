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
   and set up jenkins;
   - install jenkins-plugin xunit
   - approve what is needed to avoid errors

5. go to
      http://localhost/portainer
   or
      http://www.trainamon.com/portainer
   and set up portainer (especially new password)

6. set up and connect slave. You can use docker host as slave. Proceed e.g. as follows:
   - create jenkins:jenkins user on slave with home directory
   - use this directory as directory for tests
   - in case of ssh issues:
     sudo apt remove openssh-client openssh-server
     sudo apt install openssh-client openssh-server
   - install java on slave
