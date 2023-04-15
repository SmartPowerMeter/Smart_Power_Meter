pipeline {

  agent {label "main"}

  options {
    buildDiscarder logRotator(daysToKeepStr: '10')
    disableConcurrentBuilds()
    timeout(time: 5, unit: 'HOURS')
  }

  stages {
    stage('GitHub Sync') {
      steps{
        sshagent(credentials : ['ssh_id']) {
          sh """
          ssh -o StrictHostKeyChecking=no root@smartpowermeter.ge 'cd /home/projects/Smart_Power_Meter && git pull'
          """
        }
      }
    }
    stage('Nginx') {
      when {
        branch "main"
        changeset "Software/Nginx/**"
      }
      steps{
        sshagent(credentials : ['ssh_id']) {
          sh """
          ssh -o StrictHostKeyChecking=no root@smartpowermeter.ge 'cd /home/projects/Smart_Power_Meter && docker compose restart webserver'
          """
        }
      }
    }
    stage('Build and Deploy second_site') {
      when {
        branch "main"
        changeset "Software/second_site/**"
      }
      steps{
        sshagent(credentials : ['ssh_id']) {
          sh """
          ssh -o StrictHostKeyChecking=no root@smartpowermeter.ge 'cd /home/projects/Smart_Power_Meter/Software/second_site && docker build -t smart_power_meter-second_site . && docker compose up -d second_site'
          """
        }
      }
    }
    stage('Build and Deploy SPM_MQTT') {
      when {
        branch "main"
        changeset "Software/SPM_MQTT/**"
      }
      steps{
        sshagent(credentials : ['ssh_id']) {
          sh """
          ssh -o StrictHostKeyChecking=no root@smartpowermeter.ge 'cd /home/projects/Smart_Power_Meter/Software/SPM_MQTT && docker build -t smart_power_meter-mqtt_broker . && docker compose up -d mqtt_broker'
          """
        }
      }
    }
    stage('Build and Deploy SPM.Api') {
      when {
        branch "main"
        changeset "Software/SPM.Api/**"
      }
      steps{
        sshagent(credentials : ['ssh_id']) {
          sh """
          ssh -o StrictHostKeyChecking=no root@smartpowermeter.ge 'cd /home/projects/Smart_Power_Meter/Software/SPM.Api/SPM.Api && docker build -t smart_power_meter-web . && docker compose up -d web'
          """
        }
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
      steps{
        sshagent(credentials : ['ssh_id']) {
          sh """
          ssh -o StrictHostKeyChecking=no root@smartpowermeter.ge 'docker rmi \$(docker images -f "dangling=true" -q)'
          """
        }
      }
    }
  }
}
// echo "" > /home/pipes/spm_pipe