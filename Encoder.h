//
// Created by corey on 4/8/22.
//

#ifndef H264_PRACTICE_ENCODER_H
#define H264_PRACTICE_ENCODER_H

#include <queue>
#include <opencv2/core/mat.hpp>

extern "C"
{
#include "x264_config.h"
#include "x264.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#include "libavutil/pixfmt.h"
}

class Encoder {
public:
    Encoder(int& width, int& height, float& fps) {

        width_ = width;
        height_ = height;
        fps_ = fps;

        x264_param_default_preset(&parameters, "veryfast", "zerolatency");
        parameters.i_log_level = X264_LOG_INFO;
        parameters.i_threads = 1;
        parameters.i_width = this->width_;
        parameters.i_height = this->height_;
        parameters.i_fps_num = (int)this->fps_;
        parameters.i_fps_den = 1;
        parameters.i_keyint_max = (int)this->fps_;
        parameters.b_intra_refresh = 1;
        parameters.rc.i_rc_method = X264_RC_CRF;
        parameters.rc.f_rf_constant = this->fps_ - 5;
        parameters.rc.f_rf_constant_max = this->fps_ + 5;
        parameters.i_sps_id = 7;
        parameters.i_log_level = X264_LOG_NONE;

        // the following two value you should keep 1
        parameters.b_repeat_headers = 1;    // to get header before every I-Frame
        parameters.b_annexb = 1; // put start code in front of nal. we will remove start code later
        x264_param_apply_profile(&parameters, "baseline");

        encoder = x264_encoder_open(&parameters);
        x264_picture_alloc(&picture_in, X264_CSP_I420, parameters.i_width, parameters.i_height);
        picture_in.i_type = X264_TYPE_AUTO;
        picture_in.img.i_csp = X264_CSP_I420;

        // i have initilized my color space converter for BGR24 to YUV420 because my opencv video capture gives BGR24 image. You can initilize according to your input pixelFormat
        convert_context = sws_getContext(parameters.i_width,
                                         parameters.i_height,
                                         AV_PIX_FMT_BGR24,
                                         parameters.i_width,
                                         parameters.i_height,
                                         AV_PIX_FMT_YUV420P,
                                         SWS_FAST_BILINEAR,
                                         nullptr,
                                         nullptr,
                                         nullptr);

    }


    int encode_frame(cv::Mat& frame) {
        int srcStride = parameters.i_width * 3;
        sws_scale(convert_context, &(frame.data), &srcStride, 0, parameters.i_height, picture_in.img.plane, picture_in.img.i_stride);
        x264_nal_t* nals ;
        int i_nals = 0;
        int frameSize = -1;


        int offset = 0;

        frameSize = x264_encoder_encode(encoder, &nals, &i_nals, &picture_in, &picture_out);
        if(frameSize > 0)
        {
            for(int i = 0; i < i_nals; i++)
            {
                output_queue.push(nals[i]);
                offset+=nals[i].i_payload;
            }
        }

        if (offset != frameSize) {
            std::cout << "not equal offset = " << offset << " framesize " << frameSize << std::endl;
        }

        return frameSize;

    }

    bool is_available_output_queue() {
        return (!output_queue.empty());
    }

    x264_nal_t getNalUnit() {
        x264_nal_t nal;
        nal = output_queue.front();
        output_queue.pop();
        return nal;
    }

    std::size_t queue_size() {
        return output_queue.size();
    }

private:
    SwsContext* convert_context;
    std::queue<x264_nal_t> output_queue;
    x264_param_t parameters{};
    x264_picture_t picture_in{}, picture_out{};
    x264_t* encoder;
    int width_{};
    int height_{};
    float fps_{};
};

#endif //H264_PRACTICE_ENCODER_H
