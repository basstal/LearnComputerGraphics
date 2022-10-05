// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>
#include <numeric>

using namespace std;
rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

static bool insideTriangle(float x, float y, const Vector3f* _v)
{   
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    Vector2f v0 = Vector2f(_v[1].x() - _v[0].x(), _v[1].y() - _v[0].y());
    Vector2f v1 = Vector2f(_v[2].x() - _v[1].x(), _v[2].y() - _v[1].y());
    Vector2f v2 = Vector2f(_v[0].x() - _v[2].x(), _v[0].y() - _v[2].y());
    Vector2f targetFromV0 = Vector2f(x - _v[0].x(), y - _v[0].y());
    Vector2f targetFromV1 = Vector2f(x - _v[1].x(), y - _v[1].y());
    Vector2f targetFromV2 = Vector2f(x - _v[2].x(), y - _v[2].y());
    float v0CrossTarget = v0.x() * targetFromV0.y() - targetFromV0.x() * v0.y();
    float v1CrossTarget = v1.x() * targetFromV1.y() - targetFromV1.x() * v1.y();
    float v2CrossTarget = v2.x() * targetFromV2.y() - targetFromV2.x() * v2.y();
    return (v0CrossTarget >= 0 && v1CrossTarget >= 0 && v2CrossTarget >= 0) || (v0CrossTarget <= 0 && v1CrossTarget <= 0 && v2CrossTarget <= 0);
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    auto v = t.toVector4();
    
    // TODO : Find out the bounding box of current triangle.
    // iterate through the pixel and find if the current pixel is inside the triangle
    // cout << "point 0 : " << v[0] << endl;
    // cout << "point 1 : " << v[1] << endl;
    // cout << "point 2 : " << v[2] << endl;
    float min_x = floor(min(v[0].x(), min(v[1].x(), v[2].x())));
    float min_y = floor(min(v[0].y(), min(v[1].y(), v[2].y())));
    float max_x = ceil(max(v[0].x(), max(v[1].x(), v[2].x())));
    float max_y = ceil(max(v[0].y(), max(v[1].y(), v[2].y())));

    // cout << "aabb : (" << min_x << ", " << min_y << ") (" << max_x << ", " << max_y << ")" << endl;
    const Vector2f sample_offsets[4] = {
        Vector2f{0.25f, 0.25f},
        Vector2f{0.75f, 0.25f},
        Vector2f{0.25f, 0.75f},
        Vector2f{0.75f, 0.75f},
    };
    
    for(int i_x = min_x; i_x < max_x; ++ i_x){
        for(int i_y = min_y; i_y < max_y; ++ i_y){
            int sample_inside_count = 0;
            int index = get_index(i_x, i_y);
            for(int sample_index = 0; sample_index < super_sample; ++sample_index)
            {
                Vector2f offset = sample_offsets[sample_index];
                float x = i_x + offset.x();
                float y = i_y + offset.y();
                if (insideTriangle(x, y, t.v))
                {
                    // If so, use the following code to get the interpolated z value.
                    auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
                    float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;
                    if ( z_interpolated < super_sample_depth_buf[index][sample_index])
                    {
                        super_sample_depth_buf[index][sample_index] = z_interpolated;
                        super_sample_frame_buf[index][sample_index] = t.getColor();
                        sample_inside_count ++;
                    }
                }
            }
            if (sample_inside_count > 0)
            {
                std::vector<float> v = super_sample_depth_buf[index];
                float sum = std::accumulate(v.begin(), v.end(),
                                decltype(v)::value_type(0));
                std::vector<Vector3f> colorV = super_sample_frame_buf[index];
                Vector3f color = Vector3f{0, 0, 0};
                for(auto &v : colorV)
                {
                    color += v;
                }
                // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
                set_pixel(Vector3f(i_x, i_y, sum / super_sample), color / super_sample);
                // cout << "draw one pixel" << endl;
            }
        }
    }


}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
        std::vector<Vector3f> super_sample_pixel_frame_buf = std::vector<Vector3f>(super_sample);
        std::fill(super_sample_pixel_frame_buf.begin(), super_sample_pixel_frame_buf.end(), Eigen::Vector3f{0, 0, 0});
        std::fill(super_sample_frame_buf.begin(), super_sample_frame_buf.end(), super_sample_pixel_frame_buf);
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
        std::vector<float> super_sample_pixel_depth_buf = std::vector<float>(super_sample);
        std::fill(super_sample_pixel_depth_buf.begin(), super_sample_pixel_depth_buf.end(), std::numeric_limits<float>::infinity());
        std::fill(super_sample_depth_buf.begin(), super_sample_depth_buf.end(), super_sample_pixel_depth_buf);
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
    super_sample_depth_buf.resize(w * h);
    super_sample_frame_buf.resize(w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}

// clang-format on