#include "report.h"

bool Report::sendMemoryReport(MemoryInfo* data)
{
	std::string url = Config::URL_REPOSITORY + XorStr("/report.php?mode=memory");

	url = url + XorStr("&base_address=") + data->base_address;
	url = url + XorStr("&region_size=") + data->region_size;
	url = url + XorStr("&characteristics=") + data->characteristics;
	url = url + XorStr("&name=") + data->name;
	url = url + XorStr("&hash=") + data->hash;

	// client information
	url = url + XorStr("&sessionid=") + Session::getSessionID();
	url = url + XorStr("&username=") + Core::getUsername();

	Network::void_sendRequest(url.c_str());
	
	Log::write(XorStr("[REPORT] %s"), url);

	return true;
}

bool Report::sendModuleReport(ModuleInfo* data)
{
	std::string url = Config::URL_REPOSITORY + XorStr("/report.php?mode=module");

	url = url + XorStr("&name=") + data->name;
	url = url + XorStr("&path=") + data->path;
	url = url + XorStr("&hash=") + data->hash;
	url = url + XorStr("&lastChange=") + data->lastChange;
	url = url + XorStr("&permission=") + data->permission;
	url = url + XorStr("&base_address=") + data->base_address;
	url = url + XorStr("&publisher=") + data->publisher;
	url = url + XorStr("&size=") + data->size;

	// client information
	url = url + XorStr("&sessionid=") + Session::getSessionID();
	url = url + XorStr("&username=") + Core::getUsername();

	Network::void_sendRequest(url.c_str());

	Log::write(XorStr("[REPORT] %s"), url);

	return true;
}

bool Report::sendWindowReport(WindowInfo *data)
{
	std::string url = Config::URL_REPOSITORY + XorStr("/report.php?mode=window");

	url = url + XorStr("&title=") + data->title;
	url = url + XorStr("&hwnd=") + data->hwnd;
	url = url + XorStr("&visible=") + data->visible;
	url = url + XorStr("&description=") + data->description;
	url = url + XorStr("&path=") + data->path;

	// client information
	url = url + XorStr("&sessionid=") + Session::getSessionID();
	url = url + XorStr("&username=") + Core::getUsername();

	printf("%s\n", url.c_str());

	Network::void_sendRequest(url.c_str());

	Log::write(XorStr("[REPORT] %s"), url);

	return true;
}

bool Report::sendCleoReport(CleoInfo* data)
{
	std::string url = Config::URL_REPOSITORY + XorStr("/report.php?mode=cleo");

	url = url + XorStr("&filename=") + data->filename;
	url = url + XorStr("&path=") + data->path;
	url = url + XorStr("&hash=") + data->hash;
	url = url + XorStr("&lastChange=") + data->lastChange;
	url = url + XorStr("&size=") + data->size;
	url = url + XorStr("&permissions=") + data->permissions;

	// client information
	url = url + XorStr("&sessionid=") + Session::getSessionID();
	url = url + XorStr("&username=") + Core::getUsername();

	Network::void_sendRequest(url.c_str());

	Log::write(XorStr("[REPORT] %s"), url);

	return true;
}

bool Report::sendVMReport(VMInfo *data)
{
	std::string url = Config::URL_REPOSITORY + XorStr("/report.php?mode=vm");

	url = url + XorStr("&name=") + data->name;

	// client information
	url = url + XorStr("&sessionid=") + Session::getSessionID();
	url = url + XorStr("&username=") + Core::getUsername();

	Network::void_sendRequest(url.c_str());

	Log::write(XorStr("[REPORT] %s"), url);

	return true;
}

bool Report::sendSFReport(SFInfo* data)
{
	std::string url = Config::URL_REPOSITORY + XorStr("/report.php?mode=sampfuncs");

	url = url + XorStr("&filename=") + data->filename;
	url = url + XorStr("&path=") + data->path;
	url = url + XorStr("&hash=") + data->hash;
	url = url + XorStr("&lastChange=") + data->lastChange;
	url = url + XorStr("&size=") + data->size;
	url = url + XorStr("&permissions=") + data->permissions;

	// client information
	url = url + XorStr("&sessionid=") + Session::getSessionID();
	url = url + XorStr("&username=") + Core::getUsername();

	Network::void_sendRequest(url.c_str());

	Log::write(XorStr("[REPORT] %s"), url);

	return true;
}

bool Report::sendDebuggerReport(DebuggerInfo* data)
{
	std::string url = Config::URL_REPOSITORY + XorStr("/report.php?mode=debugger");

	url = url + XorStr("&method=") + data->method;

	// client information
	url = url + XorStr("&sessionid=") + Session::getSessionID();
	url = url + XorStr("&username=") + Core::getUsername();

	Network::void_sendRequest(url.c_str());

	Log::write(XorStr("[REPORT] %s"), url);

	return true;
}

bool Report::sendErrorReport(ErrorInfo* data)
{
	std::string url = Config::URL_REPOSITORY + XorStr("/report.php?mode=error");

	url = url + XorStr("&error=") + data->error;
	url = url + XorStr("&call=") + data->call;
	url = url + XorStr("&ret=") + data->ret;
	url = url + XorStr("&where=") + data->wh;

	// client information
	url = url + XorStr("&username=") + Core::getUsername();

	printf("%s\n", url.c_str());
	Network::void_sendRequest(url.c_str());

	Log::write(XorStr("[REPORT] %s"), url);

	return true;
}