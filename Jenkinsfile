import java.text.SimpleDateFormat

def buildEnvImageName = "tmcenv-" + new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss").format(new Date())

pipeline {
    agent { label 'dockerHost' }

    stages {
        stage('Build-Env-Docker') {
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
		// sh 'docker run --rm -v $(pwd):/tmcombi -v $HOME/.ccache:/root/.ccache tmcenv echo /root/.ccache'
		sh 'docker run --rm -v $(pwd):/src/workspace -v /tmp/build_output:/tmp/build_output -w /src/workspace tmcenv --output_user_root=/tmp/build_output build //main:hello-world'
	    }
        }
        stage('Test') {
            steps {
                echo 'Running unit test'
		sh 'docker run --rm --env XML_OUTPUT_FILE=result.xml -v $(pwd):/src/workspace -v /tmp/build_output:/tmp/build_output -w /src/workspace tmcenv --output_user_root=/tmp/build_output test --test_verbose_timeout_warnings test:hello-test'
	    }
        }
    }
    post {
        always {
            xunit (
	    	 tools: [ GoogleTest(pattern: 'bazel-testlogs/test/hello-test/test.xml') ]
            )
        }
    }
}
