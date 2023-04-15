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
        sh '''
          echo "git --git-dir=/home/projects/Smart_Power_Meter/.git --work-tree=/home/projects/Smart_Power_Meter/ pull" > /home/pipes/spm_pipe
        '''
      }
    }
    stage('Build and Deploy second_site') {
      when {
        branch "main"
        changeset "Software/second_site/**"
      }
      steps {
        sh '''
          echo "docker build -t smart_power_meter-second_site /home/projects/Smart_Power_Meter/Software/second_site" > /home/pipes/spm_pipe
          echo "docker compose -f /home/projects/Smart_Power_Meter/docker-compose.yml up -d second_site" > /home/pipes/spm_pipe
          echo "docker rmi $(docker images -f 'dangling=true' -q)" > /home/pipes/spm_pipe
        '''
      }
    }
  }
}
// echo "" > /home/pipes/spm_pipe