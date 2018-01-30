#include <string>
#include <nanogui/glcanvas.h>
#include <nanogui/glutil.h>
#include <glad/glad.h>
#include <Eigen/Core>
#include "VideoView.h"

using std::string;
using nanogui::GLCanvas;
using nanogui::GLShader;
using Eigen::MatrixXf;
using Eigen::Vector2f;
using MatrixXu = Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic>;

VideoView::VideoView(Widget * parent)
    : GLCanvas(parent)
    , _glslVertex{ R"(
        #version 330 core
        in vec2 position;
        in vec2 coord;
        uniform vec2 scaleFactor;
        out vec2 texCoord;
        void main()
        {
            gl_Position = vec4(position.x*scaleFactor.x, position.y*scaleFactor.y, 0.0, 1.0);
            texCoord = coord;
        })" }
    , _glslFragment{ R"(
        #version 330 core
        out vec4 fragColor;
        in vec2 texCoord;
        uniform sampler2D frame;
        void main()
        {
            fragColor = texture(frame, texCoord);
        })" }
    , _frameQueue(1)
{
    _shader.init("VideoViewShader", _glslVertex, _glslFragment);

    // positions
    MatrixXf positions(2, 4);
    positions.col(0) << -1,  1; // top left 
    positions.col(1) <<  1,  1; // top right
    positions.col(2) << -1, -1; // bottom left
    positions.col(3) <<  1, -1; // bottom right

    // texture coords
    MatrixXf coords(2, 4);
    coords.col(0) << 0, 0; // top left 
    coords.col(1) << 1, 0; // top right
    coords.col(2) << 0, 1; // bottom left
    coords.col(3) << 1, 1; // bottom right

    // indices
    MatrixXu indices(3, 2);
    indices.col(0) << 0, 1, 2;  // first triangle
    indices.col(1) << 2, 3, 1;  // first triangle

    // claim and bind to buffers
    _shader.bind();
    _shader.uploadAttrib("position", positions);
    _shader.uploadAttrib("coord", coords);
    _shader.uploadIndices(indices);

    // load and create a texture 
    glGenTextures(1, &_textureid);
    glBindTexture(GL_TEXTURE_2D, _textureid); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

VideoView::~VideoView()
{
    _shader.free();
}

void VideoView::setFrame(rs2::frame frame)
{
    _frameQueue.enqueue(std::move(frame));
}

void VideoView::drawGL()
{
    // dequeue a frame from queue
    rs2::video_frame frame = _frameQueue.wait_for_frame().as<rs2::video_frame>();
    int frameWidth = frame.get_width();
    int frameHeight = frame.get_height();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.get_data());
    glGenerateMipmap(GL_TEXTURE_2D);

    // calculate scale factor
    float viewer_ratio = (float)this->width() / (float)this->height();
    // video frames always have W/H ratio > 1.0
    float frame_ratio = (float)frameWidth / (float)frameHeight;
    float ratio_diff = frame_ratio - viewer_ratio;

    Vector2f scaleFactor = Vector2f::Ones();
    // ignore if ratio difference is small
    if (abs(ratio_diff) > 0.00001)
    {
        if (ratio_diff > 0.0)
            // screen width is smaller than expected ratio, shrink the height to match
            scaleFactor(1) = width() / (frame_ratio * height());
        else
            // screen height is smaller than expected ratio, shrink the width to match
            scaleFactor(0) = (frame_ratio * height()) / width();
    }

    _shader.bind();
    _shader.setUniform("scaleFactor", scaleFactor);

    glEnable(GL_DEPTH_TEST);
    // Draw 2 triangles starting at index 0
    _shader.drawIndexed(GL_TRIANGLES, 0, 2);
    glDisable(GL_DEPTH_TEST);
}
