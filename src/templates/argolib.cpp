#include <list>
#include <string>
#include <utility>

#include <eoepca/argo/eoepcaargo.hpp>
#include <graph.hpp>
#include <zconf.h>
#include <memory>
#include "argoconfigparser.hpp"

#include "zooargo.hpp"


/***
 * replaces substrings in a string
 * @param str
 * @param from
 * @param to
 * @return
 */
static std::string innerReplace(std::string &str, const std::string &from,
                                const std::string &to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos +=
                to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}






extern "C" int start(mods::ArgoInterface::ArgoWorkflowConfig &awConfig, const std::string &cwlContent, std::list<std::pair<std::string, std::unique_ptr<mods::ArgoInterface::tgInput>>> &inputList, const std::string &uuidBaseID, const std::string &runId, std::string &id) {

    std::cerr<<awConfig.argoConfigFile<<std::endl;
    proc_ades_argo::model::ArgoWorkflowConfigParsed argoWorkflowConfigData = nlohmann::json::parse(awConfig.argoConfigFile);

    std::string newRunId(runId);
    std::string newUuidBaseID(uuidBaseID);
    innerReplace(newRunId, ".", "-");
    innerReplace(newRunId, "_", "-");
    innerReplace(newUuidBaseID, ".", "-");
    innerReplace(newUuidBaseID, "_", "-");
    std::cerr << "CWL ==================="<<std::endl;

    std::cerr << cwlContent <<std::endl;

    std::cerr << " ==================="<<std::endl;
    for (auto &a : inputList) {
        std::cerr << "Inputs: " << a.first << " " << a.second->id << " " << a.second->value
                  << " " << a.second->mimeType << "\n";
    }


    std::cerr << "start1"<<std::endl;
    // argolib
    auto argoLib = std::make_unique<EOEPCA::EOEPCAargo>(awConfig.eoepcaargoPath);

    std::cerr << "start2"<<std::endl;
    // parsing Graph
    auto cwl_graph = std::make_unique<Graph>();
    cwl_graph->loadCwlFileContent(cwlContent);


    //--------------------
    // WORKFLOW
    std::cerr << "start3"<<std::endl;
    // retrieve the step. We make the assumption that we only deal with a single step
    auto cwl_workflow_list = cwl_graph->getWorkflowList();
    auto cwl_workflow = cwl_workflow_list.front();
    if (cwl_workflow.getSteps().empty()) {
        throw std::runtime_error("cwl does not contain steps");
    }
    auto step = cwl_workflow.getSteps().front();

    std::cerr << "start3"<<std::endl;
    // step name
    std::string stepName = step.getLabel();

    std::cerr << "start4"<<std::endl;
    //step inputs
    auto inputs = step.getIn();


    //------------------------------
    // COMMAND LINE TOOL
    std::cerr << "start5"<<std::endl;
    auto cwl_commandLineToolList = cwl_graph->getCommandLineToolList();
    auto cwl_commandLineTool = cwl_commandLineToolList.front();

    std::cerr << "start6"<<std::endl;
    // basecommand
    std::string baseCommand = cwl_commandLineTool.getBaseCommand();

    // docker
    std::string dockerImageName = cwl_commandLineTool.getHints().front().getDockerRequirement().getDockerPull();

    //---------------------
    // APPLICATION
    std::cerr << "start7"<<std::endl;
    std::unique_ptr<proc_comm_lib_argo::Application> application = std::make_unique<proc_comm_lib_argo::Application>();
    application->setRunId(newRunId);
    application->setUuidBaseId(newUuidBaseID);
    application->setDockerImage(dockerImageName);
    application->setCommand(baseCommand);
    application->setUseShell(true);
    bool useStageIn = false;
    for (auto const &inputPair : inputList) {
        if (inputPair.second->mimeType.compare("application/atom+xml") == 0) { useStageIn = true; }
        application->addParam(inputPair.second->id, inputPair.second->value);
    }

    std::cerr << "start7"<<std::endl;
    if (useStageIn) {
        // pre processing node
        std::unique_ptr<proc_comm_lib_argo::NodeTemplate> stageInApplication = std::make_unique<proc_comm_lib_argo::NodeTemplate>();
        stageInApplication->setDockerImage(*argoWorkflowConfigData.get_stage()->get_in()->get_docker());
        stageInApplication->setUseShell(true);
        stageInApplication->setCommand("stagein");
        application->setPreProcessingNode(stageInApplication);
    }


    std::unique_ptr<proc_comm_lib_argo::NodeTemplate> stageOutApplication = std::make_unique<proc_comm_lib_argo::NodeTemplate>();
    stageOutApplication->setDockerImage(*argoWorkflowConfigData.get_stage()->get_out()->get_docker());
    stageOutApplication->setUseShell(true);

    std::string resultFolder= newRunId+"-"+newUuidBaseID;
    stageOutApplication->setCommand("eoepca_webdav_client.py -d " + *argoWorkflowConfigData.get_stage()->get_out()->get_webdav_endpoint() + "/ -p /" + resultFolder + "/ -a $SECRET_USERNAME:$SECRET_PASSWORD -s ");
    stageOutApplication->setIncludeTee(true);

    std::map<std::string, std::pair<std::string, std::string>> secretEnvVars;
    secretEnvVars.insert({"SECRET_USERNAME",std::make_pair("eoepcawebdavsecret","username")});
    secretEnvVars.insert({"SECRET_PASSWORD",std::make_pair("eoepcawebdavsecret","password")});
    stageOutApplication->setSecrerEnvVars(secretEnvVars);

    application->setPostProcessingNode(stageOutApplication);

    std::cerr << "start8.1"<<std::endl;
    // temporary hardcoded
    std::map<std::string, std::string> volume;
    volume["volumeName"] = "workdir";
    volume["persistentVolumeClaimName"] = "eoepca-pvc";
    volume["volumeMountPath"] = "/tmp/eoepca";
    application->setVolume(volume);


    std::string yaml;
    argoLib->create_workflow_yaml_from_app(application.get(),yaml);
    std::cerr<<yaml<<std::endl;

    std::string argoNamespace = "default";
    proc_comm_lib_argo::model::Workflow workflow;

    std::cerr << "start9"<<std::endl;
    argoLib->submit_workflow(application.get(), argoNamespace, workflow, *argoWorkflowConfigData.get_argo_url());

    std::cerr << "start10"<<std::endl;
    id = workflow.get_metadata()->get_name()->c_str();

    std::cerr << "start11"<<std::endl;
    return 0;
}

extern "C" int getStatus(mods::ArgoInterface::ArgoWorkflowConfig &awConfig, const std::string &argoWorkfloId, int &percent, std::string &message) {
    proc_ades_argo::model::ArgoWorkflowConfigParsed argoWorkflowConfigData = nlohmann::json::parse(awConfig.argoConfigFile);

    std::cerr<<"getstatus1"<<std::endl;
    // argolib
    auto argoLib = std::make_unique<EOEPCA::EOEPCAargo>(awConfig.eoepcaargoPath);

    std::string argoNamespace = "default";
    proc_comm_lib_argo::model::Workflow workflow;
    try {
        std::cerr<<"getstatus11"<<std::endl;
        argoLib->get_workflow_from_name(argoWorkfloId, argoNamespace, workflow, *argoWorkflowConfigData.get_argo_url());
        std::cerr<<"getstatus12"<<std::endl;
        if ( workflow.get_status() && workflow.get_status()->get_phase()) {
            message = *workflow.get_status()->get_phase();
        } else {
            message="Retrieving status of the workflow";
            return 1;
        }
        std::cerr<<"getstatus13"<<std::endl;
    } catch (const std::exception &ex) {
        std::cerr << ex.what() << std::endl;
        message = ex.what();
        throw std::runtime_error(message);
    } catch (const std::string &ex) {
        std::cerr << ex << std::endl;
        message = ex;
        throw std::runtime_error(message);
    } catch (...) {
        std::string error = "An error occured while retrieve the status of workflow " + argoWorkfloId;
        std::cerr << error << std::endl;
        message = error;
        throw std::runtime_error(message);
    }

    std::cerr<<"getstatus2"<<std::endl;
    int totNumberOfWorkflows = workflow.get_status()->get_nodes()->size();
    int completedWorkflows = 0;
    std::string succeededString = "Succeeded";
    for (auto const node : *workflow.get_status()->get_nodes()) {
        if (succeededString.compare(node.second.get_phase()->c_str()) == 0) {
            completedWorkflows++;
        }
    }
    percent = completedWorkflows * 100 / totNumberOfWorkflows;
    if(percent==100){
        return 0;
    }
    return 1;
}

extern "C" int getResults(mods::ArgoInterface::ArgoWorkflowConfig &awConfig, const std::string &argoWorkflowId, std::list<std::pair<std::string, std::string>> &outPutList) {


    proc_ades_argo::model::ArgoWorkflowConfigParsed argoWorkflowConfigData = nlohmann::json::parse(awConfig.argoConfigFile);
    // argolib
    auto argoLib = std::make_unique<EOEPCA::EOEPCAargo>(awConfig.eoepcaargoPath);

    std::string argoNamespace = "default";
    argoLib->get_workflow_results_from_name(argoWorkflowId, argoNamespace, outPutList, *argoWorkflowConfigData.get_argo_url(), *argoWorkflowConfigData.get_k8_url());

    return 0;
}
