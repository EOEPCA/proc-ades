pipeline {
  agent any
  environment {
      VERSION_TYPE = getTypeOfVersion(env.BRANCH_NAME)
      CONFIGURATION = getConfiguration(env.BRANCH_NAME)
  }
  stages {
    stage('Build & Publish Docker') {
      steps {
        script {
          def descriptor = readDescriptor()
          def mType=getTypeOfVersion(env.BRANCH_NAME)
          def testsuite = docker.build(descriptor.docker_image_name + ":${mType}" + descriptor.docker_image_version, "--no-cache .")
          testsuite.tag("${mType}latest")
          docker.withRegistry('https://registry.hub.docker.com', 'dockerhub-emmanuelmathot') {
            testsuite.push("${mType}" + descriptor.docker_image_version)
            testsuite.push("${mType}latest")
          }
        }
      }
    }    
  }
}

def getTypeOfVersion(branchName) {
  def matcher = (branchName =~ /master/)
  if (matcher.matches())
    return ""
  
  return "dev"
}

def getConfiguration(branchName) {
  def matcher = (branchName =~ /master/)
  if (matcher.matches())
    return "Release"
  
  return "Debug"
}

def readDescriptor (){
  return readYaml(file: 'build.yml')
}

def getVersionFromCsProj (csProjFilePath){
  def file = readFile(csProjFilePath) 
  def xml = new XmlSlurper().parseText(file)
  return xml.PropertyGroup.Version[0].text()
}



