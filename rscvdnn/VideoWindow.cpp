#include <string>
#include <nanogui/layout.h>
#include "VideoWindow.h"

using std::string;
using nanogui::Widget;
using nanogui::Window;
using nanogui::BoxLayout;
using nanogui::Orientation;
using nanogui::Alignment;

VideoWindow::VideoWindow(Widget * parent, const string & title)
    : Window(parent, title)
{
    setLayout(new BoxLayout(Orientation::Vertical, Alignment::Middle));
    _videoview = new VideoView(this);
    _videoview->setBackgroundColor(theme()->mWindowFillFocused);
    requestFocus();
}

void VideoWindow::setVideoFrame(rs2::frame frame)
{
    _videoview->setFrame(frame);
}

void VideoWindow::setSize(const Eigen::Vector2i & size)
{
    mSize = size;
    _videoview->setSize(size);
}
