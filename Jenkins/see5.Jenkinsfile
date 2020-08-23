pipeline {
    agent { label 'dockerHost' }

    stages {
       stage('see5 adult orig') {
            steps {
                echo 'Training see5 for the original adult data set'
		        sh 'docker run --rm -v $(pwd)/../data:/workspace -w /workspace c50 /c5.0 -f adult'
		        sh 'docker run --rm -v $(pwd)/../data:/workspace -w /workspace c50 /c5.0 -t 4 -f adult'
		        sh 'docker run --rm -v $(pwd)/../data:/workspace -w /workspace c50 /c5.0 -t 8 -f adult'
            }
        }
       stage('see5 adult transformed') {
            steps {
                echo 'Training see5 for the original adult data set'
                sh 'docker run --rm -v $(pwd)/../data:/workspace -w /workspace c50 /c5.0 -f adult_transformed'
		        sh 'docker run --rm -v $(pwd)/../data:/workspace -w /workspace c50 /c5.0 -t 4 -f adult_transformed'
		        sh 'docker run --rm -v $(pwd)/../data:/workspace -w /workspace c50 /c5.0 -t 8 -f adult_transformed'
            }
        }
    }
}