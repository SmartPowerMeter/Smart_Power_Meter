pipeline {

  agent {label "main"}

  options {

    buildDiscarder logRotator(artifactDaysToKeepStr: '15', artifactNumToKeepStr: '5', daysToKeepStr: '', numToKeepStr: '5')
    disableConcurrentBuilds()   

  }

  stages {

    stage('Hello') {

      steps {

        sh '''
          java -version
        '''

      }

    }

    stage('cat README') {

      when {

        branch "main"

      }

      steps {

        sh '''
          cat README.md
          echo hello world
          echo oe
        '''

      }

    }

  }

}
