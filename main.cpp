#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include "Encoder.h"
#include "Decoder.h"

int main() {
    cv::Mat image;
    int width = 1280;
    int height = 720;
    float fps = 30.0;
    cv::namedWindow("Display window");
    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    cap.set(cv::CAP_PROP_FPS, fps);
    cap.open(0);
    //int in_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    //int in_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    bool image_ready = false;

    auto *encoder = new Encoder(width, height, fps);
    auto *decoder = new Decoder(width, height);

    if (!cap.isOpened()) {
        std::cout << "cannot open camera";
    }

    while (true) {
        cap >> image;
        int frame_size = encoder->encode_frame(image);
        auto encoded_frame = std::make_unique<std::uint8_t[]>(frame_size);


        int offset = 0;
        int queue_size = encoder->queue_size();
        for (int i = 0; i < queue_size; i++) {
            x264_nal_t nal = encoder->getNalUnit();
            std::memcpy(encoded_frame.get() + offset, nal.p_payload, nal.i_payload);
            offset+=nal.i_payload;
        }

        decoder->decode(encoded_frame.get(), frame_size);

        if (offset != frame_size) {
            std::cout << "not Equal" << std::endl;
        }

        cv::Mat image_ = decoder->getMat();
        imshow("Display window", image_);
        cv::waitKey(1);
    }
    return 0;
}
