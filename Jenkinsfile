pipeline {

  agent {label "main"}

  options {
    buildDiscarder logRotator(daysToKeepStr: '10')
    disableConcurrentBuilds()
    timeout(time: 5, unit: 'HOURS')
  }

  stages {
    stage('GitHub Sync') {
      steps {
        sh 'pwd'
        sh 'whoami'
        sh 'cd /home/pipes/ && ls -l'
        sh '''
          cd /home/pipes/ && echo "git --git-dir=/home/projects/Smart_Power_Meter/.git --work-tree=/home/projects/Smart_Power_Meter/ pull" > spm_pipe
        '''
      }
    }
    // stage('Build and Deploy') {
    //   when {
    //     branch "main"
    //   }
    //   steps {
    //     sh '''
    //       cat README.md
    //       echo hello world
    //       echo oe
    //     '''
    //   }
    // }
  }
}
