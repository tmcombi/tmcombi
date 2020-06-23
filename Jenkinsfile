import java.text.SimpleDateFormat

def buildBazelEnvImageName = "tmc-bazel-env-" + new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss").format(new Date())
def buildCmakeEnvImageName = "tmc-cmake-env-" + new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss").format(new Date())

pipeline {
    agent { label 'dockerHost' }

    stages {
        stage('Build-Bazel-Env-Docker') {
            steps {
                echo 'Creating a docker container with bazel build environment'
		dir('envBazel') {		    
		    sh 'docker build --tag tmc-bazel-env .'
		    sh "docker image tag tmc-bazel-env localhost/v2/$buildBazelEnvImageName"
		    sh "docker push localhost/v2/$buildBazelEnvImageName"
		    sh "docker rmi localhost/v2/$buildBazelEnvImageName"
		}
            }
        }
       stage('Build-Cmake-Env-Docker') {
            steps {
                echo 'Creating a docker container with cmake build environment'
		dir('envCmake') {		    
		    sh 'docker build --tag tmc-cmake-env .'
		    sh "docker image tag tmc-cmake-env localhost/v2/$buildCmakeEnvImageName"
		    sh "docker push localhost/v2/$buildCmakeEnvImageName"
		    sh "docker rmi localhost/v2/$buildCmakeEnvImageName"
		}
            }
        }
        stage('Bazel-Build') {
            steps {
                echo 'Building'
		// ccache: https://linux.die.net/man/1/ccache
		// sh 'docker run --rm -v $(pwd):/tmcombi -v $HOME/.ccache:/root/.ccache tmcenv echo /root/.ccache'
		sh 'docker run --rm -v $(pwd):/src/workspace -v /tmp/build_output:/tmp/build_output -w /src/workspace tmc-bazel-env --output_user_root=/tmp/build_output build ...'
	    }
        }
        stage('Test') {
            steps {
                echo 'Running unit test'
		sh 'docker run --rm --env XML_OUTPUT_FILE=result.xml -v $(pwd):/src/workspace -v /tmp/build_output:/tmp/build_output -w /src/workspace tmc-bazel-env --output_user_root=/tmp/build_output test --test_verbose_timeout_warnings test:hello-test'
		sh 'bazel-bin/test/boost-test --log_format=XML --log_sink=results.xml --log_level=all --report_level=detailed'
	    }
        }
    }
    post {
        always {
            xunit (
	    	 tools: [ GoogleTest(pattern: 'bazel-testlogs/*/*/*.xml') ],
		 thresholds: [ skipped(failureThreshold: '0'), failed(failureThreshold: '0') ]
            )
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
