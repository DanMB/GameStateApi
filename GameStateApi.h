#pragma once
#pragma comment( lib, "BakkesMod.lib" )

#include <set>
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include "json.hpp"

#include "bakkesmod/plugin/bakkesmodplugin.h"

using websocketpp::connection_hdl;

class GameStatePlugin : public BakkesMod::Plugin::BakkesModPlugin
{
	typedef websocketpp::server<websocketpp::config::asio> PluginServer;
	typedef std::set<connection_hdl, std::owner_less<connection_hdl>> ConnectionSet;

public:
	virtual void onLoad();
	virtual void onUnload();
private:

	void OnCarDemoed(CarWrapper car, void* params, std::string funcName);
	void UpdateGameState();

	bool isPlaying = false;
	
	// Server logic
	PluginServer* ws_server;
	ConnectionSet* ws_connections;
	void RunWsServer();
	void OnHttpRequest(connection_hdl hdl);
	void SendWsPayload(string payload);
	void OnWsMsg(connection_hdl hdl, PluginServer::message_ptr msg);
	void OnWsOpen(connection_hdl hdl) { this->ws_connections->insert(hdl); }
	void OnWsClose(connection_hdl hdl) { this->ws_connections->erase(hdl); }
};