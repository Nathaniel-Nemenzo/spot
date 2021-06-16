/*
  robot_command.h: includes client and interface for communication with the robot-command service 
*/

#ifndef ROBOT_COMMAND_H
#define ROBOT_COMMAND_H

#include "bosdyn/api/robot_command_service.grpc.pb.h"
#include "bosdyn/api/geometry.grpc.pb.h"
#include "bosdyn/api/lease_service.grpc.pb.h"
#include <spot/base.h>

using bosdyn::api::RobotCommandService;
using bosdyn::api::RobotCommandRequest;
using bosdyn::api::RobotCommandResponse;
using bosdyn::api::RobotCommandResponse_Status;
using bosdyn::api::RobotCommandFeedbackRequest;
using bosdyn::api::RobotCommandFeedbackResponse;
using bosdyn::api::ClearBehaviorFaultRequest;
using bosdyn::api::ClearBehaviorFaultResponse;
using bosdyn::api::RobotCommand;
using bosdyn::api::RobotCommandFeedback;
using bosdyn::api::FullBodyCommand;
using bosdyn::api::FullBodyCommand_Feedback;
using bosdyn::api::StopCommand;
using bosdyn::api::StopCommand_Feedback;
using bosdyn::api::FreezeCommand;
using bosdyn::api::FreezeCommand_Feedback;
using bosdyn::api::SelfRightCommand;
using bosdyn::api::SelfRightCommand_Feedback;
using bosdyn::api::SafePowerOffCommand;
using bosdyn::api::SafePowerOffCommand_Feedback;
using bosdyn::api::MobilityCommand;
using bosdyn::api::MobilityCommand_Feedback;
using bosdyn::api::SE2TrajectoryCommand;
using bosdyn::api::SE2TrajectoryCommand_Feedback;
using bosdyn::api::SE2VelocityCommand;
using bosdyn::api::SE2VelocityCommand_Feedback;
using bosdyn::api::SE2VelocityCommand_Request;
using bosdyn::api::SitCommand;
using bosdyn::api::SitCommand_Feedback;
using bosdyn::api::StandCommand;
using bosdyn::api::StandCommand_Feedback;
using bosdyn::api::Lease;
using bosdyn::api::LeaseUseResult;

const extern std::string ROBOT_COMMAND_CLIENT_NAME;

class RobotCommandClient : public BaseClient<RobotCommandService> {
public:
  RobotCommandClient(const std::string &authority, const std::string &token);

  RobotCommandResponse robotCommand(Lease lease, RobotCommand command, std::string &clockIdentifier);
  RobotCommandResponse robotCommandAsync(Lease lease, RobotCommand command, std::string &clockIdentifier);
  RobotCommandFeedbackResponse robotCommandFeedback(uint32_t robotCommandId);
  RobotCommandFeedbackResponse robotCommandFeedbackAsync(uint32_t robotCommandId);
  ClearBehaviorFaultResponse clearBehaviorFault(Lease lease, uint32_t behaviorFaultId);
  ClearBehaviorFaultResponse clearBehaviorFaultAsync(Lease lease, uint32_t behaviorFaultId);

private:
  //new
  RobotCommandRequest getRobotCommandRequest(Lease lease, RobotCommand command);
  RobotCommandFeedbackRequest getRobotCommandFeedbackRequest(uint32_t robotCommandId);
  ClearBehaviorFaultRequest getClearBehaviorFaultRequest(Lease lease, uint32_t behaviorFaultId);
};

#endif
