#include "save-decode-h264.hpp"

SaveDecodeH264::SaveDecodeH264(string stream_path = "", int duration = 0)
{
    this->duration = duration;

    demuxer = FFHDDemuxer::create_ffmpeg_demuxer(stream_path);
    if (demuxer == nullptr)
    {
        INFOE("demuxer create failed");
        return;
    }

    decoder = FFHDDecoder::create_cuvid_decoder(
        false, FFHDDecoder::ffmpeg2NvCodecId(demuxer->get_video_codec()), -1, 0);

    if (decoder == nullptr)
    {
        INFOE("decoder create failed");
        return;
    }

    demuxer->get_extra_data(&packet_data, &packet_size);
    decoder->decode(packet_data, packet_size);

    // delete (packet_data);
}

void SaveDecodeH264::save_h264()
{
    FILE *outfp = fopen("output.h264", "ab+");
    FILE *outsz = fopen("outputsize.txt", "a+");
    FILE *outpts = fopen("outputpts.txt", "a+");

    do
    {
        duration++;

        demuxer->demux(&packet_data, &packet_size, &demuxpts);
        if (duration % 3 == 0)
        {
            demuxpts = (0x01ul << 60) | demuxpts;
        }
        else if (duration % 3 == 1)
        {
            demuxpts = (0x02ul << 60) | demuxpts;
        }
        else if (duration % 3 == 2)
        {
            demuxpts = (0x03ul << 60) | demuxpts;
        }

        if (packet_size == 0)
            demuxpts = 0;

        size_t size = fwrite(packet_data, 1, packet_size, outfp);
        size_t size_sz = fwrite(&packet_size, 1, sizeof(int), outsz);
        size_t size_pts = fwrite(&demuxpts, 1, sizeof(uint64_t), outpts);
        if (size != packet_size)
        {
            INFO("fwrite failed-> write:%u, pkt_size:%u\n", size, packet_size);
        }

    } while (duration < 50);
    fclose(outfp);
    fclose(outsz);
    fclose(outpts);
    // delete (packet_data);
}

void SaveDecodeH264::decode_h264()
{
    FILE *outfp_ = NULL;
    outfp_ = fopen("output.h264", "rb");
    FILE *outsz_ = NULL;
    outsz_ = fopen("outputsize.txt", "r");
    FILE *outpts_ = NULL;
    outpts_ = fopen("outputpts.txt", "r");
    uint64_t pts;

    if (outfp_ == NULL || outsz_ == NULL || outpts_ == NULL)
    {
        INFO("nothing get in file---------");

        fclose(outfp_);
        fclose(outsz_);
        fclose(outpts_);
        // delete (&packet_data);
        return;
    }

    do
    {
        duration--;

        // 读文件异常

        fread(&packet_size, 1, sizeof(int), outsz_);
        // cout << "packet_size:" << packet_size << endl;
        fread(&demuxpts, 1, sizeof(uint64_t), outpts_);
        // cout << "time_pts:" << demuxpts << endl;
        packet_data = new uint8_t[packet_size];
        size_t nnn = fread(packet_data, 1, packet_size, outfp_);
        // cout << "packet_data:" << packet_data << endl;
        // cout << "大小：" << nnn << endl;

        int ndecoded_frame = decoder->decode(packet_data, packet_size, demuxpts);

        for (int i = 0; i < ndecoded_frame; ++i)
        {
            unsigned int frame_index = 0;

            /* 因为decoder获取的frame内存，是YUV-NV12格式的。储存内存大小是 [height * 1.5] * width byte
            因此构造一个height * 1.5,  width 大小的空间
            然后由opencv函数，把YUV-NV12转换到BGR，转换后的image则是正常的height, width, CV_8UC3
            */
            // cv::Mat image(decoder->get_height() * 1.5, decoder->get_width(), CV_8U, decoder->get_frame(&pts, &frame_index));
            cv::Mat image(decoder->get_height(), decoder->get_width(), CV_8UC3, decoder->get_frame(&pts, &frame_index));
            // cv::Mat image(decoder->get_height(), decoder->get_width(), CV_8UC3);
            // cv::cvtColor(image, image, cv::COLOR_YUV2BGR_NV12);

            // decoder->get_frame(&pts, &frame_index);

            frame_index = frame_index + 1;
            INFO("write imgs/img_%05d.jpg  %dx%d", frame_index, image.cols, image.rows);
            cv::imwrite(cv::format("imgs/img_%06d.jpg", frame_index), image);
            INFO("%d, %ld", frame_index + 1, pts);
        }
        delete (packet_data);
    } while (duration >= 0);

    fclose(outfp_);
    fclose(outsz_);
    fclose(outpts_);
}

// int
// app_hard_decode()
// {

//     // save_h264("rtsp://admin:a1234567@ten01.adapdo.com:6002/Streaming/Channels/102");
//     return 0;
// }