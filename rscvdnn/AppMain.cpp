#include <string>
#include <Poco/Util/Option.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/AbstractConfiguration.h>
#include <nanogui/common.h>
#include <nanogui/object.h>
#include "AppMain.h"
#include "MainWindow.h"

using std::string;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;

// static members initialize
Poco::Event AppMain::_eventTerminated;
Poco::NotificationQueue AppMain::_queue;

BOOL AppMain::ConsoleCtrlHandler(DWORD ctrlType)
{
    switch (ctrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
        terminate();
        return _eventTerminated.tryWait(3000) ? TRUE : FALSE;
    default:
        return FALSE;
    }
}

void AppMain::terminate()
{
    _queue.enqueueUrgentNotification(new Event_TerminateRequest);
}

void AppMain::handleOptionHelp(const string & option, const string & argument)
{
    poco_trace(logger(), "handleOptionHelp: " + option + "=" + argument);
    _helpRequested = true;
    // display help
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("Test program for OpenCV DNN object detection with RealSense camera");
    helpFormatter.format(std::cout);
    // stop further processing
    stopOptionsProcessing();
}

void AppMain::initialize(Application & self)
{
    // hide the console window after command line options are handled
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    poco_information(logger(), config().getString("application.baseName", name()) + " initialize");
    // load default configuration file
    loadConfiguration();
    // all registered subsystems are initialized in ancestor's initialize procedure
    Application::initialize(self);
}

void AppMain::uninitialize()
{
    poco_information(logger(), config().getString("application.baseName", name()) + " uninitialize");
    // ancestor uninitialization
    Application::uninitialize();
}

void AppMain::defineOptions(Poco::Util::OptionSet & options)
{
    Application::defineOptions(options);

    options.addOption(
        Option("help", "h", "(/h) display help information on command line arguments")
        .required(false)
        .repeatable(false)
        .callback(OptionCallback<AppMain>(this, &AppMain::handleOptionHelp)));
}

int AppMain::main(const ArgVec & args)
{
    if (_helpRequested)
        return Application::EXIT_USAGE;

    try
    {
        // initialize GUI
        nanogui::init();
        {
            nanogui::ref<MainWindow> guiMain = new MainWindow(Eigen::Vector2i(1280, 720), "RealSense OpenCV DNN object detection");
            guiMain->drawAll();
            guiMain->setVisible(true);
            poco_information(logger(), "MainWindow started");
            nanogui::mainloop(30);
        }
        nanogui::shutdown();
    }
    catch (std::exception& e)
    {
        poco_error(logger(), string(e.what()));
        return Application::EXIT_SOFTWARE;
    }

    return Application::EXIT_OK;
}

bool AppMain::helpRequested()
{
    return _helpRequested;
}
