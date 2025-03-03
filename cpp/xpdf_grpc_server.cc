#include <string>
#include <sstream>
#include <iostream>

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "xpdf_service.grpc.pb.h"
#include "extract.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using xpdf::GetExtractedTextRequest;
using xpdf::GetExtractedTextResponse;
using xpdf::PdfToText;

class PdfToTextImpl final : public PdfToText::Service
{
public:
    explicit PdfToTextImpl(){}

    Status GetExtractedText(ServerContext *context, const GetExtractedTextRequest *req, GetExtractedTextResponse *resp) override
    {
        std::string sData = req->content();
        std::stringstream outstream;
        extractPdfText(sData, &outstream);
        resp->set_content(outstream.str());
        return Status::OK;
    }
};

void RunServer()
{
    std::string server_address("0.0.0.0:50051");
    PdfToTextImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char **argv)
{
    RunServer();
    return 0;
}