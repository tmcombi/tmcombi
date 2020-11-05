pipeline {
    agent { label 'dockerHost' }

    stages {
       stage('Get data') {
            steps {
                echo 'Downloading data: adult income prediction'
                sh 'for suffix in data names test; \
                do \
                    rm -fr adult.${suffix}*; \
                    wget https://archive.ics.uci.edu/ml/machine-learning-databases/adult/adult.$suffix; \
                done'
                sh 'sed -i "s/fnlwgt/case weight/g" adult.names'
            }
        }
        stage('Transform data') {
            steps {
                echo 'Transforming data: adult income prediction'
                sh 'python3 python/transform2monotone.py'
            }
        }
        stage('Remove non-complete') {
                    steps {
                        echo 'Remove non-complete data'
                        sh 'fgrep -v ? adult_transformed.data > adult_transformed.data_'
                        sh 'fgrep -v ? adult_transformed.test > adult_transformed.test_'
                        sh 'mv adult_transformed.data_ adult_transformed.data'
                        sh 'mv adult_transformed.test_ adult_transformed.test'
                   }
                }
    }
}
