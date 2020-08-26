pipeline {
    agent { label 'dockerHost' }

    stages {
       stage('see5 tmcombi paper') {
            steps {
                echo 'Training see5 for the data set from tmcombi paper'
                sh 'docker run --rm -v $(pwd)/data/tmc_paper:/workspace -w /workspace c50 /c5.0 -f tmc_paper'
                sh 'docker run --rm -v $(pwd)/data/tmc_paper:/workspace -w /workspace c50 /c5.0 -t 4 -f tmc_paper'
                sh 'docker run --rm -v $(pwd)/data/tmc_paper:/workspace -w /workspace c50 /c5.0 -t 512 -f tmc_paper'
            }
       }
       stage('see5 adult orig') {
            steps {
                echo 'Training see5 for the original adult data set'
		        sh 'docker run --rm -v $(pwd)/../data-manipulations:/workspace -w /workspace c50 /c5.0 -f adult'
		        sh 'docker run --rm -v $(pwd)/../data-manipulations:/workspace -w /workspace c50 /c5.0 -t 4 -f adult'
		        sh 'docker run --rm -v $(pwd)/../data-manipulations:/workspace -w /workspace c50 /c5.0 -t 512 -f adult'
            }
        }
       stage('see5 adult transformed') {
            steps {
                echo 'Training see5 for the original adult data set'
                sh 'docker run --rm -v $(pwd)/../data-manipulations:/workspace -w /workspace c50 /c5.0 -f adult_transformed'
		        sh 'docker run --rm -v $(pwd)/../data-manipulations:/workspace -w /workspace c50 /c5.0 -t 4 -f adult_transformed'
		        sh 'docker run --rm -v $(pwd)/../data-manipulations:/workspace -w /workspace c50 /c5.0 -t 512 -f adult_transformed'
            }
        }
    }
}