pipeline {
    agent { label 'dockerHost' }

    stages {
       stage('Get data') {
            steps {
                echo 'Downloading data: adult income prediction'
                sh 'wget https://archive.ics.uci.edu/ml/machine-learning-databases/adult/adult.data'
                sh 'wget https://archive.ics.uci.edu/ml/machine-learning-databases/adult/adult.names'
                sh 'wget https://archive.ics.uci.edu/ml/machine-learning-databases/adult/adult.test'
                sh 'sed -i "s/fnlwgt/case weight/g" adult.names'
            }
        }
        stage('Transform data') {
            steps {
                echo 'Transforming data: adult income prediction'
                sh 'python3 python/data.py'
            }
        }
    }
}
