//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture
{
private:
    cv::Mat image_data;

public:
    Texture(const std::string &name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(std::round(std::clamp(v_img, 0.f, (float)(width - 1))), std::round(std::clamp(u_img, 0.f, (float)(height - 1))));
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    Eigen::Vector3f getColorBilinear(float u, float v)
    {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        float u_clamped = std::clamp(v_img, 0.f, (float)(width - 1));
        float v_clamped = std::clamp(u_img, 0.f, (float)(height - 1));
        int u_floor = std::floor(u_clamped);
        int v_floor = std::floor(v_clamped);
        int u_ceil = std::ceil(u_clamped);
        int v_ceil = std::ceil(v_clamped);
        // ** back to getColor
        if (u_ceil == u_floor && v_ceil == v_floor)
        {
            auto color = image_data.at<cv::Vec3b>(u_ceil, v_ceil);
            return Eigen::Vector3f(color[0], color[1], color[2]);
        }
        else
        {
            if (u_ceil == u_floor)
            {
                u_floor = u_floor > 0 ? u_floor - 1 : u_floor;
                u_ceil = u_floor == 0 ? u_ceil + 1 : u_ceil;
            }
            if (v_ceil == v_floor)
            {
                v_floor = v_floor > 0 ? v_floor - 1 : v_floor;
                v_ceil = v_floor == 0 ? v_ceil + 1 : v_ceil;
            }
            // ** do bilinear interpolation
            auto color0 = image_data.at<cv::Vec3b>(u_floor, v_floor);
            auto color1 = image_data.at<cv::Vec3b>(u_floor, v_ceil);
            auto color2 = image_data.at<cv::Vec3b>(u_ceil, v_floor);
            auto color3 = image_data.at<cv::Vec3b>(u_ceil, v_ceil);
            float h = v_clamped - v_floor;
            float w = u_clamped - u_floor;
            color0 = interpolate(color0, color1, h);
            color1 = interpolate(color2, color3, h);
            color0 = interpolate(color0, color1, w);
            return Eigen::Vector3f(color0[0], color0[1], color0[2]);
        }
    }

    cv::Vec3b interpolate(const cv::Vec3b &v0, const cv::Vec3b &v1, float interpolatorVal)
    {
        _ASSERT(interpolatorVal >= 0 && interpolatorVal <= 1.0f);
        return v0 + (v1 - v0) * interpolatorVal;
    }
};
#endif // RASTERIZER_TEXTURE_H
