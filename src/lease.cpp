#include <spot/lease.h>

LeaseClient::LeaseClient(const std::string &root, const std::string &server) {
	// create options
  	grpc::SslCredentialsOptions opts;
  	opts.pem_root_certs = root;

	// create channel arguments
  	grpc::ChannelArguments channelArgs;
  	channelArgs.SetSslTargetNameOverride("lease.spot.robot"); // put into kv map later
  	stub_ = LeaseService::NewStub(grpc::CreateCustomChannel(server, grpc::SslCredentials(opts), channelArgs));
}

AcquireLeaseResponse LeaseClient::acquire(const std::string &resource) {
  // Data we are sending to the server.
  AcquireLeaseRequest request;
  request.set_resource(resource);
  request.mutable_header()->mutable_request_timestamp()->CopyFrom(TimeUtil::GetCurrentTime());
  request.mutable_header()->set_client_name("anything");

  // Container for the data we expect from the server.
  AcquireLeaseResponse reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->AcquireLease(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    std::cout << "Status: " << reply.status() << ", LeaseClient Acquired: " << reply.lease().resource() <<
    ", New Owner: " << reply.lease_owner().user_name() << std::endl;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
  }
  return reply;  
}

AcquireLeaseResponse LeaseClient::acquireAsync(const std::string &resource){
    // Data we are sending to the server.
  AcquireLeaseRequest request;
  request.set_resource(resource);
  request.mutable_header()->mutable_request_timestamp()->CopyFrom(TimeUtil::GetCurrentTime());
  request.mutable_header()->set_client_name("anything");

  // Container for the data we expect from the server.
  AcquireLeaseResponse reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  CompletionQueue cq;
  std::unique_ptr<ClientAsyncResponseReader<AcquireLeaseResponse>> rpc(stub_->AsyncAcquireLease(&context, request, &cq));

  // status
  Status status;
  rpc->Finish(&reply, &status, (void*)1);

  // Act upon its status.
  if (status.ok()) {
    std::cout << "Status: " << reply.status() << ", LeaseClient Acquired: " << reply.lease().resource() <<
    ", New Owner: " << reply.lease_owner().user_name() << std::endl;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
  }
  return reply;
}

TakeLeaseResponse LeaseClient::take(const std::string &resource){
  // Data we are sending to the server.
  TakeLeaseRequest request;
  request.set_resource(resource);
  request.mutable_header()->mutable_request_timestamp()->CopyFrom(TimeUtil::GetCurrentTime());
  request.mutable_header()->set_client_name("anything");

  // Container for the data we expect from the server.
  TakeLeaseResponse reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->TakeLease(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    std::cout << "Status: " << reply.status() << ", LeaseClient Acquired: " << reply.lease().resource() <<
    ", New Owner: " << reply.lease_owner().user_name() << std::endl;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
  }
  return reply;
}

TakeLeaseResponse LeaseClient::takeAsync(const std::string &resource){
  // Data we are sending to the server.
  TakeLeaseRequest request;
  request.set_resource(resource);
  request.mutable_header()->mutable_request_timestamp()->CopyFrom(TimeUtil::GetCurrentTime());
  request.mutable_header()->set_client_name("anything");

  // Container for the data we expect from the server.
  TakeLeaseResponse reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  CompletionQueue cq;
  std::unique_ptr<ClientAsyncResponseReader<TakeLeaseResponse>> rpc(stub_->AsyncTakeLease(&context, request, &cq));

  // status
  Status status;
  rpc->Finish(&reply, &status, (void*)1);

  // Act upon its status.
  if (status.ok()) {
    std::cout << "Status: " << reply.status() << ", LeaseClient Acquired: " << reply.lease().resource() <<
    ", New Owner: " << reply.lease_owner().user_name() << std::endl;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
  }
  return reply;
}

ReturnLeaseResponse LeaseClient::returnLease(Lease* lease){
  // Data we are sending to the server.
  ReturnLeaseRequest request;
  request.set_allocated_lease(lease);
  request.mutable_header()->mutable_request_timestamp()->CopyFrom(TimeUtil::GetCurrentTime());
  request.mutable_header()->set_client_name("anything");

  // Container for the data we expect from the server.
  ReturnLeaseResponse reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->ReturnLease(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    std::cout << "Status: " << reply.status() << std::endl;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
  }
  return reply;
}

ReturnLeaseResponse LeaseClient::returnLeaseAsync(Lease* lease){
  // Data we are sending to the server.
  ReturnLeaseRequest request;
  request.set_allocated_lease(lease);
  request.mutable_header()->mutable_request_timestamp()->CopyFrom(TimeUtil::GetCurrentTime());
  request.mutable_header()->set_client_name("anything");

  // Container for the data we expect from the server.
  ReturnLeaseResponse reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  CompletionQueue cq;
  std::unique_ptr<ClientAsyncResponseReader<ReturnLeaseResponse>> rpc(stub_->AsyncReturnLease(&context, request, &cq));

  // status
  Status status;
  rpc->Finish(&reply, &status, (void*)1);

  // Act upon its status.
  if (status.ok()) {
    std::cout << "Status: " << reply.status() << std::endl;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
  }
  return reply;
}

RetainLeaseResponse LeaseClient::retainLease(Lease &lease){
  // Data we are sending to the server.
  RetainLeaseRequest request;
  request.mutable_header()->mutable_request_timestamp()->CopyFrom(TimeUtil::GetCurrentTime());
  request.mutable_header()->set_client_name("anything");

  // Container for the data we expect from the server.
  RetainLeaseResponse reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->RetainLease(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    std::cout << "Status: " << status.ok() << " LeaseClient Retained: " << reply.lease_use_result().attempted_lease().resource() << std::endl;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
  }
  return reply;
}

RetainLeaseResponse LeaseClient::retainLeaseAsync(Lease &lease){
    // Data we are sending to the server.
  RetainLeaseRequest request;
  request.mutable_header()->mutable_request_timestamp()->CopyFrom(TimeUtil::GetCurrentTime());
  request.mutable_header()->set_client_name("anything");

  // Container for the data we expect from the server.
  RetainLeaseResponse reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  CompletionQueue cq;
  std::unique_ptr<ClientAsyncResponseReader<RetainLeaseResponse>> rpc(stub_->AsyncRetainLease(&context, request, &cq));

  // status
  Status status;
  rpc->Finish(&reply, &status, (void*)1);

  // Act upon its status.
  if (status.ok()) {
    std::cout << "Status: " << status.ok() << " LeaseClient Retained: " << reply.lease_use_result().attempted_lease().resource() << std::endl;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
  }
  return reply;
}

ListLeasesResponse LeaseClient::listLeases(bool includeFullLeaseInfo){
  // Data we are sending to the server.
  ListLeasesRequest request;
  request.mutable_header()->mutable_request_timestamp()->CopyFrom(TimeUtil::GetCurrentTime());
  request.mutable_header()->set_client_name("anything");

  // Container for the data we expect from the server.
  ListLeasesResponse reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->ListLeases(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    std::cout << "Status: " << status.ok() << std::endl << "1st LeaseClient: " << reply.resources(0).resource() << std::endl;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
  }
  return reply;	
}

ListLeasesResponse LeaseClient::listLeasesAsync(bool includeFullLeasesInfo){
  // Data we are sending to the server.
  ListLeasesRequest request;
  request.mutable_header()->mutable_request_timestamp()->CopyFrom(TimeUtil::GetCurrentTime());
  request.mutable_header()->set_client_name("anything");

  // Container for the data we expect from the server.
  ListLeasesResponse reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  CompletionQueue cq;
  std::unique_ptr<ClientAsyncResponseReader<ListLeasesResponse>> rpc(stub_->AsyncListLeases(&context, request, &cq));

  // status
  Status status;
  rpc->Finish(&reply, &status, (void*)1);

  // Act upon its status.
  if (status.ok()) {
    std::cout << "Status: " << status.ok() << std::endl << "1st LeaseClient: " << reply.resources(0).resource() << std::endl;
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
  }
  return reply;
}
