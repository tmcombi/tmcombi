pipeline {
    agent { label 'dockerHost' }

    stages {
        stage('Build') {
            steps {
                echo 'Building..'
            }
        }
        stage('Test') {
            steps {
                echo 'Testing..'
            }
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'
		sh 'whoami'
                sh 'ls'
                sh 'pwd'
            }
        }
    }
}
