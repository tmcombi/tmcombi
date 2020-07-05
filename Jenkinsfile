import java.text.SimpleDateFormat

def current-date-tag = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss").format(new Date())
def buildBazelEnvImageName = "tmc-bazel-env-" + new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss").format(new Date())
def buildCMakeEnvImageName = "tmc-cmake-env-" + new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss").format(new Date())

pipeline {
    agent { label 'dockerHost' }

    stages {
        stage('Build-Bazel-Env-Docker') {
            steps {
                echo 'Creating a docker container with bazel build environment'
		dir('envBazel') {		    
		    sh 'docker build --tag tmc-bazel-env .'
		    sh "docker image tag tmc-bazel-env localhost/v2/tmc-bazel-env:$current-date-tag"
		    sh "docker push localhost/v2/tmc-bazel-env:$current-date-tag"
		    sh "docker rmi localhost/v2/tmc-bazel-env:$current-date-tag"
		}
            }
        }
       stage('Build-CMake-Env-Docker') {
            steps {
                echo 'Creating a docker container with CMake build environment'
		dir('envCMake') {		    
		    sh 'docker build --tag tmc-cmake-env .'
		    sh "docker image tag tmc-cmake-env localhost/v2/$buildCMakeEnvImageName"
		    sh "docker push localhost/v2/$buildCMakeEnvImageName"
		    sh "docker rmi localhost/v2/$buildCMakeEnvImageName"
		}
            }
        }
        stage('Bazel-Build') {
            steps {
                echo 'Building using Bazel...'
		sh 'docker run --rm -v $(pwd):/src/workspace -v /tmp/build_output:/tmp/build_output -w /src/workspace tmc-bazel-env --output_user_root=/tmp/build_output build ...'
	    }
        }
	stage('CMake-Build') {
            steps {
                echo 'Building using CMake'
		dir('bin') {
			sh 'docker run --rm -v $(pwd)/..:/src/workspace -v /usr/lib/ccache:/usr/lib/ccache -w /src/workspace/bin tmc-cmake-env cmake ../'
			sh 'docker run --rm -v $(pwd)/..:/src/workspace -v /usr/lib/ccache:/usr/lib/ccache -w /src/workspace/bin tmc-cmake-env cmake --build .'
		}
	    }
        }
        stage('Unit-Test') {
            steps {
                echo 'Running unit tests'
		sh 'bazel-bin/test/boost-test --log_format=XML --log_sink=results.xml --log_level=all --report_level=detailed'
		sh 'bin/Tutorial --log_format=XML --log_sink=bin/results.xml --log_level=all --report_level=detailed'
	    }
        }
    }
    post {
        always {
            xunit (
	    	 tools: [ BoostTest(pattern: 'results.xml') ],
		 thresholds: [ skipped(failureThreshold: '0'), failed(failureThreshold: '0') ]
            )
            xunit (
	    	 tools: [ BoostTest(pattern: 'bin/results.xml') ],
		 thresholds: [ skipped(failureThreshold: '0'), failed(failureThreshold: '0') ]
            )
	    script {
	    	 currentBuild.rawBuild.project.description = "<a href=\"$BUILD_NUMBER/statistics.html\">build $BUILD_NUMBER statistics</a>\n"
		 currentBuild.description = "<a href=\"$BUILD_NUMBER/statistics.html\">build $BUILD_NUMBER statistics</a>\n"
	    }
        }
    }
}
