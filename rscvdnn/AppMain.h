#pragma once
#include <string>
#include <Poco/Util/Application.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Event.h>
#include <Poco/Notification.h>
#include <Poco/NotificationQueue.h>

class AppMain : public Poco::Util::Application
{
private:
    // for handling Ctrl+C and terminate request
    static Poco::Event _eventTerminated;
    static BOOL __stdcall ConsoleCtrlHandler(DWORD ctrlType);
    static void terminate();
    static Poco::NotificationQueue _queue;
    // for the help request by user
    bool _helpRequested{ false };
    void handleOptionHelp(const std::string& name, const std::string& value);

protected:
    void initialize(Poco::Util::Application& self);
    void uninitialize();
    void defineOptions(Poco::Util::OptionSet& options);
    int main(const ArgVec& args);

public:
    AppMain() {};
    bool helpRequested();
};

class Event_TerminateRequest : public Poco::Notification
{
public:
    Event_TerminateRequest() {}
};
