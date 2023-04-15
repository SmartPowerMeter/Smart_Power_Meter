pipeline {

  agent {label "main"}

  options {
    buildDiscarder logRotator(daysToKeepStr: '10')
    disableConcurrentBuilds()
    timeout(time: 5, unit: 'HOURS')
  }

  stages {
    stage('GitHub Sync') {
      sshagent(credentials : ['ssh_id']) {
        sh """
        ssh -o StrictHostKeyChecking=no root@smartpowermeter.ge 'cd /home/projects/Smart_Power_Meter && git pull'
        """
      }  
      // steps {
      //   sh '''
      //     echo "git --git-dir=/home/projects/Smart_Power_Meter/.git --work-tree=/home/projects/Smart_Power_Meter/ pull" > /home/pipes/spm_pipe
      //   '''
      // }
    }
    stage('Nginx') {
      when {
        branch "main"
        changeset "Software/Nginx/**"
      }
      steps {
        sh """echo "docker compose -f /home/projects/Smart_Power_Meter/docker-compose.yml restart webserver" > /home/pipes/spm_pipe"""
      }
    }
    stage('Build and Deploy second_site') {
      when {
        branch "main"
        changeset "Software/second_site/**"
      }
      steps {
        sh """echo "docker build -t smart_power_meter-second_site /home/projects/Smart_Power_Meter/Software/second_site" > /home/pipes/spm_pipe"""
        sh """echo "docker compose -f /home/projects/Smart_Power_Meter/docker-compose.yml up -d second_site" > /home/pipes/spm_pipe"""
      }
    }
    stage('Build and Deploy SPM_MQTT') {
      when {
        branch "main"
        changeset "Software/SPM_MQTT/**"
      }
      steps {
        sh """echo "docker build -t smart_power_meter-mqtt_broker /home/projects/Smart_Power_Meter/Software/SPM_MQTT" > /home/pipes/spm_pipe"""
        sh """echo "docker compose -f /home/projects/Smart_Power_Meter/docker-compose.yml up -d mqtt_broker" > /home/pipes/spm_pipe"""
      }
    }
    stage('Build and Deploy SPM.Api') {
      when {
        branch "main"
        changeset "Software/SPM.Api/**"
      }
      steps {
        sh """echo "docker build -t smart_power_meter-web /home/projects/Smart_Power_Meter/Software/SPM.Api/SPM.Api" > /home/pipes/spm_pipe"""
        sh """echo "docker compose -f /home/projects/Smart_Power_Meter/docker-compose.yml up -d web" > /home/pipes/spm_pipe"""
      }
    }
    stage('Remove Dangling Images') {
      when {
        branch "main"
        anyOf {
          changeset "Software/second_site/**"
          changeset "Software/SPM_MQTT/**"
          changeset "Software/SPM.Api/**"
        }
      }
      steps {
        sh """echo '''docker rmi \$(docker images -f "dangling=true" -q)''' > /home/pipes/spm_pipe"""
      }
    }
  }
}
// echo "" > /home/pipes/spm_pipe