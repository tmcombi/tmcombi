import java.text.SimpleDateFormat

def buildEnvImageName = "tmcenv-" + new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss").format(new Date())

pipeline {
    agent { label 'dockerHost' }

    stages {
        stage('Docker-Env') {
            steps {
                echo 'Creating a docker container with build environment'
		dir('env') {		    
		    sh 'docker build --tag tmcenv .'
		    sh "docker image tag tmcenv localhost/v2/$buildEnvImageName"
		    sh "docker push localhost/v2/$buildEnvImageName"
		    sh "docker rmi localhost/v2/$buildEnvImageName"
		}
            }
        }
        stage('Build') {
            steps {
                echo 'Building'
		// ccache: https://linux.die.net/man/1/ccache
		sh 'docker run -v $(pwd):/tmcombi -v $HOME/.ccache:/root/.ccache tmcenv ls /root/.ccache'
		}
            }
        }
    }
