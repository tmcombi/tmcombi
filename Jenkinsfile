pipeline {
    agent { label 'dockerHost' }

    stages {
        stage('Docker-Env') {
            steps {
                echo 'Creating a docker container with build environment'
		dir('env') {
		    sh 'echo $(id -u):$(id -g)'
		    sh 'docker build --tag tmcenv --user $(id -u):$(id -g) .'
		}
            }
        }
    }
}
