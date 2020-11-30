pipeline {
  agent any
  environment {
      VERSION_TYPE = getTypeOfVersion(env.BRANCH_NAME)
      CONFIGURATION = getConfiguration(env.BRANCH_NAME)
  }
  stages {
    stage('Build ZOO') {
      script {
        def descriptor = readDescriptor()
        def mType=getTypeOfVersion(env.BRANCH_NAME)
        def testsuite = docker.build(descriptor.zoo_base_docker_image_name + ":${mType}" + descriptor.zoo_base_docker_image_version, "." )
        testsuite.tag("${mType}latest")
        // docker.withRegistry('https://docker.terradue.com', 'f7ea73e5-5715-4d47-b250-842d61825e34') {
        //   testsuite.push("${mType}${env.VERSION_TOOL}")
        //   testsuite.push("${mType}latest")
        // }
      }
    }
    // stage('Publish Artifacts') {
    //   agent { node { label 'artifactory' } }
    //   steps {
    //     echo 'Deploying'
    //     unstash name: 'cpes-rpms'
    //     script {
    //         // Obtain an Artifactory server instance, defined in Jenkins --> Manage:
    //         def server = Artifactory.server "repository.terradue.com"

    //         // Read the upload specs:
    //         def uploadSpec = readFile 'artifactdeploy.json'

    //         // Upload files to Artifactory:
    //         def buildInfo = server.upload spec: uploadSpec

    //         // Publish the merged build-info to Artifactory
    //         server.publishBuildInfo buildInfo
    //     }
    //   }       
    // }
    // stage('Build & Publish Docker') {
    //   steps {
    //     script {
    //       unstash name: 'cpes-packages'
    //       sh "ls -lrth Server/bin/*/*/*/cpes.*.rhel-x64.rpm"
    //       def cpesrpm = findFiles(glob: "Server/bin/**/*.rhel-x64.rpm")
    //       def descriptor = readDescriptor()
    //       sh "mv ${cpesrpm[0].path} ."
    //       def mType=getTypeOfVersion(env.BRANCH_NAME)
    //       def testsuite = docker.build(descriptor.docker_image_name + ":${mType}${env.VERSION_TOOL}", "--no-cache --build-arg CPES_RPM=${cpesrpm[0].name} --build-arg VERSION_PREFIX=${mType} .")
    //       testsuite.tag("${mType}latest")
    //       docker.withRegistry('https://docker.terradue.com', 'f7ea73e5-5715-4d47-b250-842d61825e34') {
    //         testsuite.push("${mType}${env.VERSION_TOOL}")
    //         testsuite.push("${mType}latest")
    //       }
    //     }
    //   }
    // }    
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



