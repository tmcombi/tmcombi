import java.text.SimpleDateFormat

def currentDateTag = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss").format(new Date())

pipeline {
    agent { label 'dockerHost' }

    stages {
        //stage('Build-Bazel-Env-Docker') {
        //    steps {
        //        echo 'Creating a docker container with bazel build environment'
		//        dir('envBazel') {
		//	    sh 'docker pull ubuntu'
		//            sh 'docker build --tag tmc-bazel-env .'
		//            sh "docker image tag tmc-bazel-env localhost/v2/tmc-bazel-env:$currentDateTag"
		//            sh "docker push localhost/v2/tmc-bazel-env:$currentDateTag"
		//            sh "docker rmi localhost/v2/tmc-bazel-env:$currentDateTag"
		//        }
        //    }
        //}
       stage('Build-CMake-Env-Docker') {
            steps {
                echo 'Creating a docker container with CMake build environment'
		        dir('envCMake') {
		            sh 'docker pull ubuntu'
		            sh 'docker build --tag tmc-cmake-env .'
		            sh "docker image tag tmc-cmake-env localhost/v2/tmc-cmake-env:$currentDateTag"
		            sh "docker push localhost/v2/tmc-cmake-env:$currentDateTag"
		            sh "docker rmi localhost/v2/tmc-cmake-env:$currentDateTag"
		        }
            }
        }
        //stage('Bazel-Build') {
        //    steps {
        //        echo 'Building using Bazel...'
		//        sh 'docker run --rm -v $(pwd):/workspace -w /workspace -v /tmp/build_output:/tmp/build_output tmc-bazel-env bazel --output_user_root=/tmp/build_output build ...'
	    //    }
        //}
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
                sh 'rm -fr bin/results*.xml'
		        sh 'bin/test_feature_vector     --log_format=XML --log_sink=bin/results_feature_vector.xml     --log_level=all --report_level=detailed'
		        sh 'bin/test_feature_names      --log_format=XML --log_sink=bin/results_feature_names.xml      --log_level=all --report_level=detailed'
		        sh 'bin/test_sample             --log_format=XML --log_sink=bin/results_sample.xml             --log_level=all --report_level=detailed'
		        sh 'bin/test_sample_creator     --log_format=XML --log_sink=bin/results_sample_creator.xml     --log_level=all --report_level=detailed'
		        sh 'bin/test_layer_partitioning --log_format=XML --log_sink=bin/results_layer_partitioning.xml --log_level=all --report_level=detailed'
		        sh 'bin/test_io                 --log_format=XML --log_sink=bin/results_test_io.xml            --log_level=all --report_level=detailed'
		        //sh 'bazel-bin/test/boost-test --log_format=XML --log_sink=results.xml --log_level=all --report_level=detailed'
	        }
        }
    }
    post {
        always {
            //xunit (
	    	//    tools: [ BoostTest(pattern: 'results.xml') ],
		    //    thresholds: [ skipped(failureThreshold: '0'), failed(failureThreshold: '0') ]
            //)
            xunit (
	    	    tools: [ BoostTest(pattern: 'bin/results*.xml') ],
		        thresholds: [ skipped(failureThreshold: '0'), failed(failureThreshold: '0') ]
            )
	        script {
	    	    currentBuild.rawBuild.project.description = "<a href=\"$BUILD_NUMBER/statistics.html\">build $BUILD_NUMBER statistics</a>\n"
		        currentBuild.description = "<a href=\"$BUILD_NUMBER/statistics.html\">build $BUILD_NUMBER statistics</a>\n"
	        }
        }
    }
}
