#pragma once
#include <string>
#include <mutex>
#include <unordered_map>
#include <Poco/Logger.h>
#include <Poco/Util/LayeredConfiguration.h>
#include <Eigen/Core>
#include <nanogui/common.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/button.h>
#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include "VideoWindow.h"

// text translation id for multilingual GUI text
enum class TextId : uint8_t
{
    ControlSetting,
    VideoStream,
    ColorStream,
    DepthStream,
    DnnObjDetect,
    StartDetect
};

// text translation mapping for multilingual GUI text
using TextMap = std::unordered_map<TextId, std::string>;

class MainWindow : public nanogui::Screen
{
public:
    MainWindow(const Eigen::Vector2i & size, const std::string & caption);
    void onToggleColorStream(bool on);
    void onToggleDepthStream(bool on);
    void onToggleCvdnn(bool on);
    bool keyboardEvent(int key, int scancode, int action, int modifiers) override;
    bool resizeEvent(const Eigen::Vector2i & size) override;
    void draw(NVGcontext *ctx) override;

protected:
    void initTextMap();
    bool tryStartVideo();
    void stopVideo();
    bool isVideoStarted();
    bool isCvdnnStarted();
    void detectObjects(rs2::video_frame color_frame, rs2::depth_frame depth_frame, float depth_scale);

private:
    Poco::Logger & _logger;
    Poco::Util::LayeredConfiguration & _config;
    TextMap _textmap;
    nanogui::Window *_settingWindow;
    nanogui::Button *_btnColorStream;
    nanogui::Button *_btnDepthStream;
    nanogui::Button *_btnStartCvdnn;
    VideoWindow *_colorWindow;
    VideoWindow *_depthWindow;
    const float _colorRatio;
    const float _depthRatio;
    std::mutex _mutex;
    bool _isVideoStarted;
    bool _isCvdnnStarted;
    rs2::pipeline _pipe;
    rs2::align _align;
    float _depthScale;
    const size_t _inWidth;
    const size_t _inHeight;
    const float _inScaleFactor;
    const float _meanVal;
    const std::array<std::string, 21> _classNames;
    cv::dnn::Net _net;
    cv::Rect _rectRoi;
    cv::Rect _rectRoiLeft;
    cv::Rect _rectRoiRight;
};
