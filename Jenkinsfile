pipeline {
    agent { label 'dockerHost' }

    stages {
        stage('Docker-Env') {
            steps {
                echo 'Creating a docker container with build environment'
		sh 'pwd'
		dir('env') {
		    sh 'pwd'
		}
            }
        }
    }
}
