#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    float rotation_radians = rotation_angle / 180.0f * acos(-1);
    float cos_rotation_radians = cos(rotation_radians);
    float sin_rotation_radians = sin(rotation_radians);
    model << cos_rotation_radians, sin_rotation_radians, 0, 0, -sin_rotation_radians, cos_rotation_radians, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;

    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.
    float half_fov_radians = (eye_fov / 2.0f) / 180.0f * acos(-1);
    float top = tan(half_fov_radians) * abs(zNear);
    float bottom = -top;
    float right = aspect_ratio * top;
    float left = -right;
    projection << 2 * zNear / (right - left), 0, (right + left) / (left - right), 0,
                    0, 2 * zNear / (top - bottom), (top + bottom) / (bottom - top), 0,
                    0, 0, (zNear + zFar) / (zNear - zFar), 2 * zNear * zFar / (zFar - zNear), 
                    0, 0, 1, 0;

    return projection;
}

Eigen::Matrix4f get_rotation(Vector3f axis, float angle)
{
    Eigen::Matrix4f free_rotation = Eigen::Matrix4f::Identity();

    float rotation_radians = (angle / 180.0f) * acos(-1);
    float cos_angle = cos(rotation_radians);
    float sin_angle = sin(rotation_radians);
    float one_minus_cos_angle = 1 - cos_angle;
    float n_x = axis[0];
    float n_y = axis[1];
    float n_z = axis[2];
    free_rotation << pow(n_x, 2) * one_minus_cos_angle + cos_angle, n_x * n_y * one_minus_cos_angle - n_z * sin_angle, n_x * n_z * one_minus_cos_angle + n_y * sin_angle, 
                    n_x * n_y * one_minus_cos_angle + n_z * sin_angle, pow(n_y, 2) * one_minus_cos_angle + cos_angle, n_y * n_z * one_minus_cos_angle - n_x * sin_angle,
                    n_x * n_z * one_minus_cos_angle - n_y * sin_angle, n_y * n_z * one_minus_cos_angle + n_x * sin_angle, pow(n_z, 2) * one_minus_cos_angle + cos_angle;


    return free_rotation;
}

int main(int argc, const char **argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3)
    {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4)
        {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a')
        {
            angle += 10;
        }
        else if (key == 'd')
        {
            angle -= 10;
        }
    }

    return 0;
}
