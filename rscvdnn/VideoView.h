#pragma once
#include <string>
#include <nanogui/glcanvas.h>
#include <nanogui/glutil.h>
#include <librealsense2/rs.hpp>

class VideoView : public nanogui::GLCanvas
{
public:
    VideoView(nanogui::Widget *parent);
    ~VideoView();
    void setFrame(rs2::frame frame);
    void drawGL() override;

private:
    nanogui::GLShader _shader;
    const std::string _glslVertex;
    const std::string _glslFragment;
    uint32_t _textureid;
    rs2::frame_queue _frameQueue;
};
