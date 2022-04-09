//
// Created by corey on 4/9/22.
//

#ifndef H264_PRACTICE_DECODER_H
#define H264_PRACTICE_DECODER_H

#include <opencv2/core/mat.hpp>
#include <iostream>

extern "C" {
#include<libavutil/avutil.h>
#include<libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

class Decoder {
public:
    Decoder(int& width, int& height) {

        image_h = height;
        image_w = width;

        rgb_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, image_w, image_h, 1);

        codec = avcodec_find_decoder(AV_CODEC_ID_H264);
        context = avcodec_alloc_context3(codec);
        context->width = image_w;
        context->height = image_h;
        context->extradata = nullptr;
        context->pix_fmt = AV_PIX_FMT_YUV420P;
        avcodec_open2(context, codec, nullptr);

        img_convert_ctx = sws_getContext(image_w, image_h, AV_PIX_FMT_YUV420P, image_w, image_h,
                                         AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR,nullptr,
                                         nullptr, nullptr);

        frame = icv_alloc_picture_FFMPEG(AV_PIX_FMT_YUV420P, image_w, image_h, true);
        pFrameBGR = icv_alloc_picture_FFMPEG(AV_PIX_FMT_RGB24, image_w, image_h, true);
    }

    static AVFrame * icv_alloc_picture_FFMPEG(int pix_fmt, int width, int height, bool alloc)
    {
        AVFrame * picture;
        uint8_t * picture_buf;
        int size;

        picture = av_frame_alloc();
        if (!picture)
            return nullptr;
        size = av_image_get_buffer_size((AVPixelFormat)pix_fmt, width, height, 1);
        if(alloc)
        {
            picture_buf = new uint8_t[size];
            if (!picture_buf)
            {
                av_frame_free(&picture);
                std::cout << "picture buff = NULL" << std::endl;
                return nullptr;
            }
            int filled = av_image_fill_arrays(picture->data, picture->linesize, picture_buf,
                                              (AVPixelFormat)pix_fmt, width, height, 1);
        }
        return picture;
    }


    int decode(unsigned char *inputbuf, int& size) {

        AVPacket av_packet;
        av_new_packet(&av_packet, size);
        av_packet.data = inputbuf;
        av_packet.size = size;
        av_packet.flags = AV_PKT_FLAG_CORRUPT;
        av_packet.pos = -1;


        avcodec_send_packet(context, &av_packet);

        int ret = avcodec_receive_frame(context, frame);

        if(ret != 0)
            return false;

        //Convert the frame from YUV420 to RGB24
        sws_scale(img_convert_ctx, frame->data, frame->linesize, 0, image_h,
                  pFrameBGR->data, pFrameBGR->linesize);

        pCvMat.create(cv::Size(image_w, image_h), CV_8UC3);

        int success = av_image_copy_to_buffer(pCvMat.data, rgb_size, pFrameBGR->data, pFrameBGR->linesize,
                                              AV_PIX_FMT_RGB24, context->width, context->height, 1);

        if (!success) {
            return 0;
        }

        //av_free(&av_packet);

        return success;

    }

    cv::Mat getMat() {
        return pCvMat;
    }


private:
    const AVCodec *codec = nullptr;
    AVCodecContext *context = nullptr;
    int frame_count{};
    AVFrame *frame;
    AVFrame *pFrameBGR;
    int image_w, image_h;
    int rgb_size{};

    struct SwsContext *img_convert_ctx{};
    cv::Mat pCvMat;

};

#endif //H264_PRACTICE_DECODER_H
