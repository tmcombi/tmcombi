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
      http://www.tmcombi.org/jenkins
   and set up jenkins:
   - install jenkins-plugin xunit
   - goto "Manage Jenkins/Configure Global Security" and switch "Markup Formatter" to "Safe HTML"

6. set up and connect jenkins slave. You can use docker host as slave. Proceed e.g. as follows:
   - install java on slave
   - create jenkins:jenkins user on slave with home directory
   - set password for jenkins: sudo passwd jenkins
   - use this directory as directory for tests
   - in case of ssh issues:
     sudo apt remove openssh-client openssh-server
     sudo apt install openssh-client openssh-server

7. Run the tests:
   - add and run a pipeline item
   - approve what is needed to avoid errors
   
8. go to
      http://localhost/portainer
   or
      http://www.tmcombi.org/portainer
   and set up portainer (especially new password)

