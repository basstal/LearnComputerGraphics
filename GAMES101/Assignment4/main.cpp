#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> control_points;
std::vector<cv::Vec3f> added_color_buf;
std::vector<int> added_color_count_buf;

int width = 700, height = 700;
void mouse_handler(int event, int x, int y, int flags, void *userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 4)
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
                  << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window)
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001)
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                     3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t)
{
    // TODO: Implement de Casteljau's algorithm
    auto points_size = control_points.size();
    if (points_size == 1) // special case ?
    {
        return control_points[0];
    }
    if (points_size > 2)
    {
        std::vector<cv::Point2f> next_control_points;
        for (auto it = control_points.begin(); it != control_points.end() - 1; ++it)
        {
            cv::Point2f p0 = *it;
            cv::Point2f p1 = *(it + 1);
            next_control_points.push_back(cv::Point2f((1.0f - t) * p0.x + t * p1.x, (1.0f - t) * p0.y + t * p1.y));
        }
        return recursive_bezier(next_control_points, t);
    }
    else
    {
        cv::Point2f p0 = control_points[0];
        cv::Point2f p1 = control_points[1];
        return cv::Point2f((1.0f - t) * p0.x + t * p1.x, (1.0f - t) * p0.y + t * p1.y);
    }
}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window)
{
    for (double t = 0.0; t <= 1.0; t += 0.001)
    {
        auto point = recursive_bezier(control_points, (float)t);
        window.at<cv::Vec3b>(point.y, point.x)[1] = 255;
    }
}

void bezier_antialiasing(const std::vector<cv::Point2f> &control_points, cv::Mat &window)
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's
    // recursive Bezier algorithm.
    for (double t = 0.0; t <= 1.0; t += 0.001)
    {
        auto point = recursive_bezier(control_points, (float)t);
        auto x_decimal_under_point_five = (int)(point.x - 0.5f) != (int)point.x;
        int x_near = x_decimal_under_point_five ? std::floor(point.x) : std::ceil(point.x);
        int x_far = x_decimal_under_point_five ? std::ceil(point.x) : std::floor(point.x);
        auto y_decimal_under_point_five = (int)(point.y - 0.5f) != (int)point.y;
        float x_near_ratio = x_decimal_under_point_five ? std::ceil(point.x) - point.x : point.x - (int)point.x;

        int y_near = y_decimal_under_point_five ? std::floor(point.y) : std::ceil(point.y);
        int y_far = y_decimal_under_point_five ? std::ceil(point.y) : std::floor(point.y);
        float y_near_ratio = y_decimal_under_point_five ? std::ceil(point.y) - point.y : point.y - (int)point.y;
        _ASSERT(x_near_ratio < 1.0f);
        int index0 = x_near + y_near * width;
        added_color_count_buf[index0] += 1;
        added_color_buf[index0][1] += x_near_ratio * 255.0f;
        int index1 = x_near + y_far * width;
        added_color_count_buf[index1] += 1;
        added_color_buf[index1][1] += x_near_ratio * 255.0f / 2;
        int index2 = x_far + y_near * width;
        added_color_count_buf[index2] += 1;
        added_color_buf[index2][1] += x_near_ratio * 255.0f / 2;
        int index3 = x_far + y_far * width;
        added_color_count_buf[index3] += 1;
        added_color_buf[index3][1] += x_near_ratio * 255.0f / 4;
    }
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            auto index = x + y * width;
            auto added_color_count = added_color_count_buf[index];
            if (added_color_count > 0)
            {
                cv::Vec3b color = added_color_buf[index] / added_color_count;
                // std::cout << color << std::endl;
                window.at<cv::Vec3b>(y, x) = color;
            }
        }
    }
}

int main()
{
    cv::Mat window = cv::Mat(width, height, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);
    
    // control_points.emplace_back(106, 459);
    // control_points.emplace_back(224, 226);
    // control_points.emplace_back(470, 208);
    // control_points.emplace_back(464, 439);

    added_color_buf.resize(width * height);
    added_color_count_buf.resize(width * height);

    int key = -1;
    while (key != 27)
    {
        for (auto &point : control_points)
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() == 4)
        {
            // naive_bezier(control_points, window);
            // bezier(control_points, window);
            std::fill(added_color_buf.begin(), added_color_buf.end(), cv::Vec3f{0, 0, 0});
            std::fill(added_color_count_buf.begin(), added_color_count_buf.end(), 0);
            bezier_antialiasing(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

    return 0;
}
