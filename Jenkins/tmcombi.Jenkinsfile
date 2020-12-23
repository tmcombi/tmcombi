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
        stage('Unit Tests') {
            steps {
                echo 'Running unit tests'
                sh 'rm -fr bin/results*.xml'
		        sh 'bin/test_feature_transform_subset  --log_format=XML --log_sink=bin/results_feature_transform_subset.xml  --log_level=all --report_level=detailed'
		        sh 'bin/test_feature_vector     --log_format=XML --log_sink=bin/results_feature_vector.xml     --log_level=all --report_level=detailed'
		        sh 'bin/test_feature_names      --log_format=XML --log_sink=bin/results_feature_names.xml      --log_level=all --report_level=detailed'
		        sh 'bin/test_sample             --log_format=XML --log_sink=bin/results_sample.xml             --log_level=all --report_level=detailed'
		        sh 'bin/test_sample_creator     --log_format=XML --log_sink=bin/results_sample_creator.xml     --log_level=all --report_level=detailed'
		        sh 'bin/test_statistics         --log_format=XML --log_sink=bin/results_statistics.xml         --log_level=all --report_level=detailed'
		        sh 'bin/test_border             --log_format=XML --log_sink=bin/results_border.xml             --log_level=all --report_level=detailed'
		        sh 'bin/test_graph_creator      --log_format=XML --log_sink=bin/results_graph_creator.xml      --log_level=all --report_level=detailed'
		        sh 'bin/test_layer_partitioner  --log_format=XML --log_sink=bin/results_layer_partitioner.xml  --log_level=all --report_level=detailed'
                sh 'bin/test_layer_partitioning --log_format=XML --log_sink=bin/results_layer_partitioning.xml --log_level=all --report_level=detailed'
                sh 'bin/test_layer_partitioning_creator --log_format=XML --log_sink=bin/results_layer_partitioning_creator.xml --log_level=all --report_level=detailed'
		        sh 'bin/test_border_system      --log_format=XML --log_sink=bin/results_border_system.xml      --log_level=all --report_level=detailed'
		        sh 'bin/test_classifier_tmc     --log_format=XML --log_sink=bin/results_classifier_tmc.xml      --log_level=all --report_level=detailed'
		        sh 'bin/test_evaluator          --log_format=XML --log_sink=bin/results_evaluator.xml          --log_level=all --report_level=detailed'
		        sh 'bin/test_train_classifier_tmc --log_format=XML --log_sink=bin/results_train_classifier_tmc.xml --log_level=all --report_level=detailed'
		        sh 'bin/test_classifier_reduced_features       --log_format=XML --log_sink=bin/results_classifier_reduced_features.xml      --log_level=all --report_level=detailed'
		        sh 'bin/test_forward_selection  --log_format=XML --log_sink=bin/results_forward_selection.xml      --log_level=all --report_level=detailed'
                //sh 'bazel-bin/test/boost-test --log_format=XML --log_sink=results.xml --log_level=all --report_level=detailed'
	        }
        }
        stage('Performance Tests') {
            steps {
                echo 'Running performance tests'
                sh 'bin/test_rtree4d                      --log_format=XML --log_sink=bin/results_test_rtree4d.xml                 --log_level=all --report_level=detailed'
        		sh 'bin/test_rtree25d                     --log_format=XML --log_sink=bin/results_test_rtree25d.xml                --log_level=all --report_level=detailed'
        		sh 'bin/test_tmc_paper_dataset            --log_format=XML --log_sink=bin/results_tmc_paper_dataset.xml            --log_level=all --report_level=detailed'
        		sh 'bin/test_tmc_paper_weighted_dataset   --log_format=XML --log_sink=bin/results_tmc_paper_weighted_dataset.xml   --log_level=all --report_level=detailed'
        		sh 'bin/test_graph_creator_26kNodes       --log_format=XML --log_sink=bin/results_graph_creator_26kNodes.xml          --log_level=all --report_level=detailed'
        		sh 'bin/test_adult_transformed_dataset    --log_format=XML --log_sink=bin/results_adult_transformed_dataset.xml    --log_level=all --report_level=detailed'
        		sh 'bin/test_fs_tmc_paper_dataset         --log_format=XML --log_sink=bin/results_fs_tmc_paper_dataset.xml         --log_level=all --report_level=detailed'
                sh 'bin/test_fs_adult_transformed_dataset --log_format=XML --log_sink=bin/results_fs_adult_transformed_dataset.xml --log_level=all --report_level=detailed'
        	}
        }
        stage('Main Tests') {
            steps {
                echo 'Running performance tests'
                sh 'bin/tmc --names data/tmc_paper/tmc_paper.names --train-data data/tmc_paper/tmc_paper.data --eval-data data/tmc_paper/tmc_paper.test --trained-config tmc_paper_border_system.json'
                sh 'bin/tmc_classify_example --trained-config tmc_paper_border_system.json'
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
