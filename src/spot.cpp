#include <spot/spot.h>

Spot::Spot() :
		_spotbase(new CoreLayer::SpotBase) 
		/*_spotpayloads(new CoreLayer::SpotPayloads), */
		//_spotcontrol(new RobotLayer::SpotControl(_spotbase)),
		//_spotstate(new RobotLayer::SpotState(_spotbase)) //,
		/* _spotdata(new RobotLayer::SpotData) */ {}

void Spot::initClients(){
	_spotcontrol = std::shared_ptr<RobotLayer::SpotControl>(new RobotLayer::SpotControl(_spotbase));
	_spotstate = std::shared_ptr<RobotLayer::SpotState>(new RobotLayer::SpotState(_spotbase));
}

void Spot::basicInit(const std::string &username, const std::string &password){
	authenticate(username, password);
	initClients();
	initBasicEstop();
	initBasicLease();
	initBasicTimeSync();
	powerOnBlocking();
}

void Spot::authenticate(const std::string &username, const std::string &password){
	_spotbase->authenticate(username, password);
}

void Spot::authenticateWithToken(const std::string &token){
	// TODO:: This method is incomplete
}

void Spot::initBasicEstop(){
	std::shared_ptr<ClientLayer::EstopEndpoint> endpoint = std::shared_ptr<ClientLayer::EstopEndpoint>(new ClientLayer::EstopEndpoint(_spotcontrol->getEstopClient(), "pdb_root", "PDB_rooted", "", "", 30, 3));
	std::set<std::shared_ptr<ClientLayer::EstopEndpoint>> endpoints;
	endpoints.insert(endpoint);
	_spotcontrol->setEstopConfiguration(endpoints);
	std::string unique_id = _spotcontrol->registerEstopEndpoint("pdb_root", "PDB_rooted", _spotcontrol->getEstopConfigId(), 30, 3);
	_spotcontrol->beginEstopping(unique_id);
}

void Spot::initBasicLease(){
	_spotcontrol->acquireLease("body");
	_spotcontrol->beginLeasing();
}

void Spot::initBasicTimeSync(){
	_spotbase->beginTimesync();
}

uint32_t Spot::powerOn(){
	return _spotcontrol->powerOnMotors();
}

void Spot::powerOnBlocking(){
	blockUntilPowerComplete(powerOn());
}

uint32_t Spot::powerOff(){
	return _spotcontrol->powerOffMotors();
}

void Spot::powerOffBlocking(){
	blockUntilPowerComplete(powerOff());
}

void Spot::blockUntilPowerComplete(uint32_t powerCommandID){
	PowerCommandFeedbackResponse pcfr = getPowerClient()->PowerCommandFeedback(powerCommandID);
    while(pcfr.status() != 2){
        pcfr = getPowerClient()->PowerCommandFeedback(powerCommandID);
        sleep(1);
    }
}

void Spot::sit(){
	_spotcontrol->sit();
}

void Spot::stand(){
	_spotcontrol->stand();
}

void Spot::velocityMove(double x, double y, double angular, int64_t time, gravAlignedFrame frame){
	_spotcontrol->velocityMove(x, y, angular, time, frame);
}

void Spot::trajectoryMove(Trajectory2D trajectory, int64_t time, gravAlignedFrame frame){
	_spotcontrol->trajectoryMove(trajectory, frame, time);
}

void Spot::setMobilityParams(MobilityParams mParams){
	_spotcontrol->setMobilityParams(mParams);
}

void Spot::setBodyPose(Trajectory3D trajectory, bool gravityAlign){
	BodyControlParams bodyParams;

    if(gravityAlign)
        bodyParams.set_rotation_setting(bosdyn::api::spot::BodyControlParams_RotationSetting_ROTATION_SETTING_ABSOLUTE);
    else
        bodyParams.set_rotation_setting(bosdyn::api::spot::BodyControlParams_RotationSetting_ROTATION_SETTING_OFFSET);
    
    bodyParams.mutable_base_offset_rt_footprint()->CopyFrom(trajectory.getTrajectory());

	MobilityParams mParams = _spotcontrol->getMobilityParams();
    mParams.mutable_body_control()->CopyFrom(bodyParams);
	_spotcontrol->setMobilityParams(mParams);
}

void Spot::resetBodyPose(double time){
	Trajectory3D traj;
    traj.addPointRPY(0,0,0,0,0,0, time);
    setBodyPose(traj, false);
}
