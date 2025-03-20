#include <string>
#include <sstream>
#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
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
using grpc::StatusCode;
using xpdf::GetExtractedTextRequest;
using xpdf::GetExtractedTextResponse;
using xpdf::PdfToText;

std::atomic<bool> shutdown_requested(false);

void SignalHandler(int signal)
{
    shutdown_requested = true;
}

class PdfToTextImpl final : public PdfToText::Service
{
public:
    explicit PdfToTextImpl() {}

    Status GetExtractedText(ServerContext *context, const GetExtractedTextRequest *req, GetExtractedTextResponse *resp) override
    {
        try
        {
            std::string sData = req->content();
            if (sData.empty())
            {
                resp->set_content("failed to extract");
                return Status(StatusCode::INVALID_ARGUMENT, "failed to extract", "request content is empty");
            }

            std::stringstream outstream;
            int ret = extractPdfText(sData, &outstream);
            if (ret == -1)
            {
                resp->set_content("failed to extract");
                return Status(StatusCode::INVALID_ARGUMENT, "failed to extract", "request contents could not be successfully read as a pdf");
            }

            resp->set_content(std::move(outstream.str()));
            return Status::OK;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return Status(StatusCode::INTERNAL, "internal server error", e.what());
        }
        catch (...)
        {
            std::cerr << "Unknown error occurred" << std::endl;
            return Status(StatusCode::INTERNAL, "internal server error", "unknown error occurred");
        }
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

    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    while (!shutdown_requested)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    server->Shutdown();
    std::cout << "Server shutting down..." << std::endl;
}

int main(int argc, char **argv)
{
    RunServer();
    return 0;
}