pipeline {
    agent { label 'dockerHost' }

    stages {
       stage('Get data') {
            steps {
                echo 'Downloading data: adult income prediction'
                sh 'for suffix in data names test; do rm -fr adult.${suffix}*; wget https://archive.ics.uci.edu/ml/machine-learning-databases/adult/adult.$suffix;done'
                //sh 'wget https://archive.ics.uci.edu/ml/machine-learning-databases/adult/adult.data'
                //sh 'wget https://archive.ics.uci.edu/ml/machine-learning-databases/adult/adult.names'
                //sh 'wget https://archive.ics.uci.edu/ml/machine-learning-databases/adult/adult.test'
                sh 'sed -i "s/fnlwgt/case weight/g" adult.names'
            }
        }
        stage('Transform data') {
            steps {
                echo 'Transforming data: adult income prediction'
                sh 'python3 python/transform2monotone.py'
            }
        }
    }
}
