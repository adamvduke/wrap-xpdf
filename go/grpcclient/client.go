package grpcclient

import (
	"context"
	"fmt"
	"time"

	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"

	pb "github.com/adamvduke/wrap-xpdf/go/xpdf-client/xpdfpb"
)

type XpdfRpcClient struct {
	Addr string
}

func (client *XpdfRpcClient) Extract(input []byte) ([]byte, error) {
	conn, err := grpc.NewClient(client.Addr, grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		return nil, err
	}
	defer conn.Close()
	c := pb.NewPdfToTextClient(conn)

	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()

	r, err := c.GetExtractedText(ctx, &pb.GetExtractedTextRequest{Content: input})
	if err != nil {
		return nil, fmt.Errorf("GetExtractedText failed: %w", err)
	}

	return r.GetContent(), nil
}
