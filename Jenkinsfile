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
		sh 'docker run --rm -v $(pwd):/src/workspace -v /tmp/build_output:/tmp/build_output -w /src/workspace tmcenv --output_user_root=/tmp/build_output build ...'
	    }
        }
        stage('Test') {
            steps {
                echo 'Running unit test'
		sh 'docker run --rm --env XML_OUTPUT_FILE=result.xml -v $(pwd):/src/workspace -v /tmp/build_output:/tmp/build_output -w /src/workspace tmcenv --output_user_root=/tmp/build_output test --test_verbose_timeout_warnings test:hello-test'
		sh 'bazel-bin/test/boost-test --log_format=XML --log_sink=results.xml --log_level=all --report_level=no'
	    }
        }
    }
    post {
        always {
            xunit (
	    	 tools: [ BoostTest(pattern: 'results.xml') ],
		 thresholds: [ skipped(failureThreshold: '0'), failed(failureThreshold: '0') ]
            )
	    script {
	    	 currentBuild.rawBuild.project.description = "<a href=\"$BUILD_NUMBER/statistics.html\">build $BUILD_NUMBER statistics</a>\n"
		 currentBuild.description = "<a href=\"$BUILD_NUMBER/statistics.html\">build $BUILD_NUMBER statistics</a>\n"
	    }
        }
    }
}
