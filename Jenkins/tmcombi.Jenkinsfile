import java.text.SimpleDateFormat

def currentDateTag = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss").format(new Date())

pipeline {
    agent { label 'dockerHost' }

    stages {
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
		        sh 'bin/unit_tests --log_format=XML --log_sink=bin/results_unit_tests.xml --log_level=all --report_level=detailed'
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
        		sh 'bin/test_fs_n_fold_tmc_paper_dataset         --log_format=XML --log_sink=bin/results_fs_n_fold_tmc_paper_dataset.xml         --log_level=all --report_level=detailed'
        		sh 'bin/test_fs_graph_tmc_paper_dataset         --log_format=XML --log_sink=bin/results_fs_graph_tmc_paper_dataset.xml         --log_level=all --report_level=detailed'
                sh 'bin/test_fs_n_fold_adult_transformed_dataset         --log_format=XML --log_sink=bin/results_fs_n_fold_adult_transformed_dataset.xml         --log_level=all --report_level=detailed'
                sh 'bin/test_fs_graph_adult_transformed_dataset         --log_format=XML --log_sink=bin/results_fs_graph_adult_transformed_dataset.xml         --log_level=all --report_level=detailed'
        		sh 'bin/test_adaboost_tmc_paper_dataset         --log_format=XML --log_sink=bin/results_adaboost_tmc_paper_dataset.xml         --log_level=all --report_level=detailed'
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
