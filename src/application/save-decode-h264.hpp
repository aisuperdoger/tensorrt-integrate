#ifndef SAVE_DECODE_H264_HPP
#define SAVE_DECODE_H264_HPP

#include <opencv2/opencv.hpp>
#include <ffhdd/simple-logger.hpp>
#include <ffhdd/ffmpeg-demuxer.hpp>
#include <ffhdd/cuvid-decoder.hpp>
#include <ffhdd/nalu.hpp>
#include <string>

using namespace std;

class SaveDecodeH264
{
public:
    SaveDecodeH264(string stream_path, int duration);
    void save_h264();
    void decode_h264();

private:
    std::shared_ptr<FFHDDemuxer::FFmpegDemuxer> demuxer;
    std::shared_ptr<FFHDDecoder::CUVIDDecoder> decoder;
    uint8_t *packet_data = nullptr;
    int packet_size = 0;
    uint64_t demuxpts = 0;
    int duration;
};

#endif