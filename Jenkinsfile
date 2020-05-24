pipeline {
    agent { label 'dockerHost' }

    stages {
        stage('Docker-Env') {
            steps {
                echo 'Creating a docker container with build environment'
		dir('env') {
		    sh 'docker build --tag tmcenv .'
		}
            }
        }
    }
}
