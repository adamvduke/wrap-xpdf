package main

import (
	"errors"
	"flag"
	"io"
	"log"
	"os"

	cgo "github.com/adamvduke/wrap-xpdf/go/cgoclient"
	rpc "github.com/adamvduke/wrap-xpdf/go/grpcclient"
)

//go:generate protoc --go_out=../ --go-grpc_out=../ --proto_path=../../proto/ ../../proto/xpdf_service.proto

type Extractor interface {
	Extract([]byte) ([]byte, error)
}

func main() {
	listenPath := flag.String("server-address", "0.0.0.0:50051", "server address hosting the extract service")
	inputPath := flag.String("input-path", "", "path to the input pdf to extract")
	outputPath := flag.String("output-path", "", "path to the input pdf to extract")
	useGrpc := flag.Bool("use-grpc", false, "if the program should attempt to extract text via grpc instead of cgo")

	flag.Parse()

	if *inputPath == "" || *outputPath == "" {
		log.Fatal("must pass both a path to a pdf to extract, and a path to write extracted text")
	}

	pdfData, err := readInput(*inputPath)
	if err != nil {
		log.Fatal(err)
	}

	var client Extractor

	if *useGrpc {
		client = &rpc.XpdfRpcClient{Addr: *listenPath}
	} else {
		client = &cgo.XpdfCgoClient{}
	}

	data, err := client.Extract(pdfData)
	if err != nil {
		log.Fatal(err)
	}

	count, err := writeOutput(*outputPath, data)
	if err != nil {
		log.Fatal(err)
	}
	if count < len(data) {
		log.Fatalf("failed to fully write response data, wrote: %d bytes", count)
	}
	log.Printf("Successfully wrote %d bytes to %s", count, *outputPath)
}

func writeOutput(path string, data []byte) (int, error) {
	f, err := os.Create(path)
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()
	return f.Write(data)
}

func readInput(path string) ([]byte, error) {
	if _, err := os.Stat(path); errors.Is(err, os.ErrNotExist) {
		log.Fatal(err)
	}
	f, err := os.Open(path)
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()
	return io.ReadAll(f)
}
