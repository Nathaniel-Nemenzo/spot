#include <spot/robot_layer.h>

namespace RobotLayer {
    SpotState::SpotState() {}

    SpotControl::SpotControl(std::shared_ptr<CoreLayer::SpotBase> spotBase) :
            _services(),
            _endpoints(),
            _estopThreads(),
            _leases(),
            _leaseThreads() {
        // initialize pointers
        std::string authToken = spotBase->getAuthToken();
        if (authToken.empty()) {
            // todo: exception handling
        }

        // initalize services map
        _services = spotBase->listAllServices();

        // initialize clients
        _estopClient = std::shared_ptr<EstopClient>(new EstopClient(_services.find(ESTOP_CLIENT_NAME)->second.getAuthority(), authToken));  
        _leaseClient = std::shared_ptr<LeaseClient>(new LeaseClient(_services.find(LEASE_CLIENT_NAME)->second.getAuthority(), authToken));        
        _powerClient = std::shared_ptr<PowerClient>(new PowerClient(_services.find(POWER_CLIENT_NAME)->second.getAuthority(), authToken));  
        _robotCommandClient = std::shared_ptr<RobotCommandClient>(new RobotCommandClient(_services.find(ROBOT_COMMAND_CLIENT_NAME)->second.getAuthority(), authToken)); 
        _spotCheckClient = std::shared_ptr<SpotCheckClient>(new SpotCheckClient(_services.find(SPOT_CHECK_CLIENT_NAME)->second.getAuthority(), authToken));  
    }

    bool SpotControl::estopped() {
        GetEstopSystemStatusResponse reply;
        try {
            reply = _estopClient->getStatus();
        } catch (Error &e) {
            std::cout << e.what() << std::endl;
            throw; // may change later depending on if this is the last stop for errors
        }

        // check if we are not at stop level none
        return reply.status().stop_level() != bosdyn::api::ESTOP_LEVEL_NONE;
    }

    void SpotControl::setEstopConfiguration(const std::set<std::shared_ptr<SpotEstopEndpoint>> &endpoints, const std::string &targetConfigId) {
        // create config
        EstopConfig config; 

        // iterate through set of endpoints
        for (auto estopEndpoint : endpoints) {
            // add endpoint
            EstopEndpoint *endpoint = config.add_endpoints();

            // only set role and name (will get replaced by actual endpoint)
            endpoint->set_role(estopEndpoint->getRole());
            // endpoint->set_name(estopEndpoint->getName());
            endpoint->mutable_timeout()->CopyFrom(TimeUtil::SecondsToDuration(estopEndpoint->getEstopTimeout()));
        }

        // set config
        SetEstopConfigResponse reply;
        try {
            reply = _estopClient->setConfig(config); 

            if (reply.status() == 2) {
                reply = _estopClient->setConfig(config, reply.active_config().unique_id());
            }
        } catch (Error &e) {
            std::cout << e.what() << std::endl;
            return;
        }

        // get config id
        std::cout << "setestopconfig status: " << reply.status() << std::endl;
        _estopConfigId = reply.active_config().unique_id();
    }

    std::string SpotControl::registerEstopEndpoint(const std::string &name, const std::string &role, const std::string &configId, 
            int64_t estopTimeout, int64_t estopCutPowerTimeout) {
        // check if we are registering to instance config or another config
        std::string temp = configId.empty() ? _estopConfigId : configId;
        
        EstopEndpoint fromEndpoint;
        // create new endpoint obj w/ blank unique id (assigned by server)
        try {
            // create new endpoint
            std::shared_ptr<SpotEstopEndpoint> ptr(new SpotEstopEndpoint(_estopClient, name, role, temp, "", estopTimeout, estopCutPowerTimeout));

            // convert to proto representation
            EstopEndpoint fromEndpoint = ptr->toProto();
            
            // create temp endpoint to replace
            EstopEndpoint endpointToReplace;
            endpointToReplace.set_role(role);

            // rpc and set new unique id
            RegisterEstopEndpointResponse reply = _estopClient->replaceEndpoint(temp, endpointToReplace, fromEndpoint);
            ptr->setUniqueId(reply.new_endpoint().unique_id());
            
            // insert into map
            _endpoints.insert(std::pair<std::string, std::shared_ptr<SpotEstopEndpoint>>(ptr->getUniqueId(), ptr));
            return ptr->getUniqueId();
        } catch (Error &e) {
            std::cout << e.what() << std::endl;
            return "";
        }
    }

    void SpotControl::deregisterEstopEndpoint(const std::string &uniqueId, const std::string &targetConfigId) {
        // check config id
        std::string temp = targetConfigId.empty() ? _estopConfigId : targetConfigId;

        // find in map
        auto it = _endpoints.find(uniqueId);
        if (it == _endpoints.end()) {
            // todo: change to exception
            std::cout << "Endpoint has not been registered or has failed to register. " << std::endl;
        }

        // deregister from spot
        EstopEndpoint endpointToDeregister = it->second->toProto();
        DeregisterEstopEndpointResponse reply;
        try {
            reply = _estopClient->deregister(temp, endpointToDeregister);
        } catch (Error &error) {
            std::cout << error.what() << std::endl;
            return;
        }

        // remove from endpoints
        _endpoints.erase(it);
    }

    void SpotControl::beginEstopping(const std::string &uniqueId) {
        auto it = _endpoints.find(uniqueId);
        if (it == _endpoints.end()) {
           // todo: exception classes 
           std::cout << "it.end()" << std::endl;
        }

        // create estop thread
        std::shared_ptr<EstopThread> ptr = std::shared_ptr<EstopThread>(new EstopThread(_estopClient, it->second));

        std::cout << "getUniqueId(): " << it->second->getUniqueId();
        // add thread to map
        _estopThreads.insert(std::pair<std::string, std::shared_ptr<EstopThread>>(it->second->getUniqueId(), ptr));

        // kick off estop
        ptr->beginEstop();
    }

    void SpotControl::beginEstopping() {
        std::cout << "in beginEstopping()" << std::endl;
        for (const auto &endpoint : _endpoints) {
            // create estop thread
            std::shared_ptr<EstopThread> ptr = std::shared_ptr<EstopThread>(new EstopThread(_estopClient, endpoint.second));

            // add thread to map
            std::cout << "_estopThreads.insert()" << std::endl;
            _estopThreads.insert(std::pair<std::string, std::shared_ptr<EstopThread>>(endpoint.second->getName(), ptr));

            // kick off estop
            std::cout << "ptr->beginEstop()" << std::endl;
            ptr->beginEstop();
        }
    }

    void SpotControl::endEstopping(const std::string &uniqueId) {
        auto it = _estopThreads.find(uniqueId);
        if (it == _estopThreads.end()) {
            // todo: exception hanoding
        }

        // end estop
        it->second->endEstop();

        // delete from thread map
        _estopThreads.erase(it);
    }

    void SpotControl::endEstopping() {
        // end estopping for all threads
        for (const auto &thread : _estopThreads) {
            thread.second->endEstop();            
        }

        // erase all entries from map
        _estopThreads.clear();
    }

    void SpotControl::acquireLease(const std::string &resource) {
        AcquireLeaseResponse reply;

        // acquire lease for given resource
        try {
            std::cout << "lease acquire rpc sent" << std::endl;
            reply = _leaseClient->acquire(resource);
        } catch (Error &error) {
            std::cout << error.what() << std::endl;
            return;
        }

        // check if already claimed
        Lease acquiredLease;
        std::cout << "acquiredLease" << std::endl;
        switch (reply.status()) {
            case 0: 
                break;
            case 1:
                acquiredLease = reply.lease();
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            default:
                ;
        }

        // add to lease map
        std::cout << "_leases.insert()" << std::endl;
        _leases.insert(std::pair<std::string, Lease>(resource, acquiredLease));
    }

    void SpotControl::beginLeasing(const std::string &resource) {
        auto it = _leases.find(resource);
        if (it == _leases.end()) {

        }

        // create thread
        std::shared_ptr<LeaseThread> thread = std::shared_ptr<LeaseThread>(new LeaseThread(_leaseClient, it->second));

        // add thread to thread map
        _leaseThreads.insert(std::pair<std::string, std::shared_ptr<LeaseThread>>(resource, thread));

        // kick off thread
        thread->beginLease();
    }

    void SpotControl::beginLeasing() {
        for (const auto &lease : _leases) {
            // create thread
            std::shared_ptr<LeaseThread> thread = std::shared_ptr<LeaseThread>(new LeaseThread(_leaseClient, lease.second));

            // add to thread map
            _leaseThreads.insert(std::pair<std::string, std::shared_ptr<LeaseThread>>(lease.first, thread));

            // kick off thread
            thread->beginLease();
        }
    }

    void SpotControl::endLeasing(const std::string &resource) {
        auto it = _leaseThreads.find(resource);
        if (it == _leaseThreads.end()) {
            // todo: exception hanoding
        }

        // end lease
        it->second->endLease();

        // delete from thread map
        _leaseThreads.erase(it);
    }

    void SpotControl::endLeasing() {
        // end leasing for all threads
        for (const auto &thread : _leaseThreads) {
            thread.second->endLease();            
        }

        // erase all entries from map
        _leaseThreads.clear();
    }

    bool SpotControl::poweredOn() {
        return false; 
    }

    void SpotControl::powerOnMotors() {
        // todo: exception handling
        // power
        PowerCommandRequest_Request pcr_r;
        pcr_r = bosdyn::api::PowerCommandRequest_Request_REQUEST_ON; // PowerCommandRequest_Request_REQUEST_OFF to turn off, change to _ON to turn on

        // get lease
        Lease bodyLease = _leases.find("body")->second;
        PowerCommandResponse powerCommResp = _powerClient->PowerCommand(bodyLease, pcr_r); 
        uint32_t pcID = powerCommResp.power_command_id();

        PowerCommandFeedbackResponse pcfr = _powerClient->PowerCommandFeedback(pcID);
	    while(pcfr.status() != 2){
		    pcfr = _powerClient->PowerCommandFeedback(pcID);
		    sleep(1);
	    }
    }

    void SpotControl::powerOffMotors() {
        // todo: exception handling
        // turn off
        PowerCommandRequest_Request pcr_r;
        pcr_r = bosdyn::api::PowerCommandRequest_Request_REQUEST_OFF;

        // get lease
        Lease bodyLease = _leases.find("body")->second;
        PowerCommandResponse powerCommResp = _powerClient->PowerCommand(bodyLease, pcr_r);
        uint32_t pcID = powerCommResp.power_command_id();

        PowerCommandFeedbackResponse pcfr = _powerClient->PowerCommandFeedback(pcID);
	    while(pcfr.status() != 2){
		    pcfr = _powerClient->PowerCommandFeedback(pcID);
		    sleep(1);
	    }
    }

    void SpotControl::sit() {

    }

    void SpotControl::stand() {

    }

}