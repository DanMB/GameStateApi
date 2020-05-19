#include "GameStateApi.h"
using websocketpp::connection_hdl;
using namespace std;
using placeholders::_1;
using placeholders::_2;
BAKKESMOD_PLUGIN(GameStatePlugin, "GameStatePlugin", "0.1.2", PLUGINTYPE_THREADED)

void GameStatePlugin::onLoad() {

	// Function TAGame.Car_TA.FinishDemo - bots tested, can only get attacker when client dies, can never get victim
	// Function TAGame.Car_TA.EventDemolished - bots tested, can only get attacker when client dies, can never get victim
	// Function TAGame.Car_TA.OnDemolished - bots tested, can only get attacker when client dies, can never get victim

	// Function TAGame.Car_TA.SetAttackerPRI - CAN ALWAYS GET VICTIM, can only get attacker when client dies - BEST SO FAR
	// Function TAGame.Car_TA.ClearAttackerPRI
	// Function TAGame.Car_TA.DemolishDestroyTimer
	// Function TAGame.Car_TA.Destroyed
	
	// Function TAGame.Car_TA.CanDemolish - No event
	// Function TAGame.Car_TA.ShouldDemolish - No event
	// Function TAGame.Car_TA.Demolish - No event

	// Function TAGame.FXActor_Boost_TA.HandleCarDemolished - Event but no data
	// Function TAGame.ProductStat_Demolishes_TA.OnStatEvent - No event
	// Function TAGame.StatFactory_TA.OnCarDemolished - No event
	// Function TAGame.AchievementManager_TA.HandlePlayerDemolished - No event
	// Function TAGame.CrowdSoundManager_TA.HandleDemolish - No event

	// Function TAGame.BTC_Stuck.IsTouchingOtherCar
	// Function TAGame.GameEvent_Soccar_TA.Active.HandleCarTouch
	// Function TAGame.GameEvent_Soccar_TA.HandleCarTouch

	//gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.OnDemolished", std::bind(&GameStatePlugin::OnCarDemoed, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	//gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.EventBumpedCar", std::bind(&GameStatePlugin::OnCarDemoed, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));


	this->ws_connections = new ConnectionSet();
	this->UpdateGameState();
	RunWsServer();
}

void GameStatePlugin::OnCarDemoed(CarWrapper car, void* params, std::string funcName)
{
	/*json::JSON event;
	event["event"] = "demoevent";

	if (car.IsNull()) {
		cvarManager->log("[Demo] Victim car is null."); // demo car is never null
		event["attacker"] = "";
		event["victim"] = "";
	}
	else {
		auto pri = car.GetPRI();
		//car.GetPreviewTeamIndex()
		cvarManager->log("[Demo] Got victim car...");
		cvarManager->log("[Demo] Victim team index: " + std::to_string(car.GetPreviewTeamIndex()));
		if (pri.IsNull()) {
			cvarManager->log("[Demo] Victim: NULL");

			event["victim"] = "";
		}
		else {
			cvarManager->log("[Demo] Victim: " + pri.GetPlayerName().ToString());
			string name = pri.GetPlayerName().IsNull() ? "" : pri.GetPlayerName().ToString();
			event["victim"] = name + "_" + std::to_string(pri.GetSpectatorShortcut());
		}
		auto att = car.GetAttackerPRI();
		if (att.IsNull()) {
			cvarManager->log("[Demo] Attacker: NULL");
			event["attacker"] = "";
		}
		else {
			cvarManager->log("[Demo] Attacker: " + att.GetPlayerName().ToString());
			string name = att.GetPlayerName().IsNull() ? "" : att.GetPlayerName().ToString();
			event["attacker"] = name + "_" + std::to_string(att.GetSpectatorShortcut());
		}
	}

	this->SendWsPayload(
		event.dump()
	);*/
}


void GameStatePlugin::UpdateGameState() {
	json::JSON state;
	state["event"] = "gamestate";
	state["players"] = json::Object();
	state["game"] = json::Object();

	int playersLen = 0;
	bool isInGame = false;

	if (gameWrapper->IsInOnlineGame()) {
		ServerWrapper server = gameWrapper->GetOnlineGame();
		if (!server.IsNull()) {
			if (gameWrapper->GetLocalCar().IsNull() && server.GetPlaylist().GetPlaylistId() == 6) {
			//if (server.GetPlaylist().GetPlaylistId() == 6) {

				isInGame = true;

				ArrayWrapper<PriWrapper> PRIs = server.GetPRIs();
				if (PRIs.Count() > 0) {
					for (int i = 0; i < PRIs.Count(); i++) {
						PriWrapper pri = PRIs.Get(i);
						if (pri.IsNull()) { continue; }
						int team = pri.GetTeamNum();
						if (pri.IsSpectator() || team == 255) { continue; }

						int key = pri.GetSpectatorShortcut();
						string name = pri.GetPlayerName().IsNull() ? "" : pri.GetPlayerName().ToString();
						string id = name + "_" + std::to_string(key);

						state["players"][id] = json::Object();

						state["players"][id]["name"] = name;
						state["players"][id]["id"] = id;
						state["players"][id]["key"] = key;
						state["players"][id]["team"] = team;
						state["players"][id]["score"] = pri.GetMatchScore();
						state["players"][id]["goals"] = pri.GetMatchGoals();
						state["players"][id]["shots"] = pri.GetMatchShots();
						state["players"][id]["assists"] = pri.GetMatchAssists();
						state["players"][id]["saves"] = pri.GetMatchSaves();
						state["players"][id]["touches"] = pri.GetBallTouches();
						state["players"][id]["cartouches"] = pri.GetCarTouches();


						CarWrapper car = pri.GetCar();
						if (car.IsNull()) {
							state["players"][id]["speed"] = 0;
							state["players"][id]["boost"] = 0;
							state["players"][id]["isSonic"] = false;
							state["players"][id]["hasCar"] = false;

							state["players"][id]["isDead"] = false;
							state["players"][id]["attacker"] = "";
						}
						else {


							//isReplay = car.GetbReplayActor() ? true : false;

							if (car.GetbHidden()) {
								state["players"][id]["isDead"] = true;
								PriWrapper att = car.GetAttackerPRI(); // Attacker is only set on local player???
								if (!att.IsNull()) {
									/*CarWrapper attCar = att.GetCar();
									if (!attCar.IsNull()) {
										cvarManager->log("[Attacker] can be demoed: " + std::to_string(attCar.CanDemolish(car)));
									}*/
									string attName = att.GetPlayerName().IsNull() ? "" : att.GetPlayerName().ToString();
									state["players"][id]["attacker"] = attName + "_" + std::to_string(att.GetSpectatorShortcut());
								}
								else {
									state["players"][id]["attacker"] = "";
								}


							}
							else {
								state["players"][id]["isDead"] = false;
								state["players"][id]["attacker"] = "";
							}

							state["players"][id]["speed"] = static_cast<int>((car.GetVelocity().magnitude() / 27.778) + 0.5); // Speed in uu/s, 1uu = 1cm, divided by 27.778 to get from cm/s to km/h
							float boost = car.GetBoostComponent().IsNull() ? 0 : car.GetBoostComponent().GetPercentBoostFull();
							state["players"][id]["boost"] = static_cast<int>(boost * 100);
							state["players"][id]["isSonic"] = car.GetbSuperSonic() ? true : false;
							state["players"][id]["hasCar"] = true;

							playersLen++;
						}
					}
				}


				if (server.GetTeams().Count() == 2) {
					TeamWrapper team0 = server.GetTeams().Get(0);
					if (!team0.IsNull()) {
						state["game"]["team0"] = team0.GetCustomTeamName().IsNull() ? "BLUE" : team0.GetCustomTeamName().ToString();
						state["game"]["score0"] = team0.GetScore();
					}
					else {
						state["game"]["team0"] = "BLUE";
						state["game"]["score0"] = 0;
					}

					TeamWrapper team1 = server.GetTeams().Get(1);
					if (!team1.IsNull()) {
						state["game"]["team1"] = team1.GetCustomTeamName().IsNull() ? "ORANGE" : team1.GetCustomTeamName().ToString();
						state["game"]["score1"] = team1.GetScore();
					}
					else {
						state["game"]["team1"] = "ORANGE";
						state["game"]["score1"] = 0;
					}
				}
				else {
					state["game"]["team0"] = "BLUE";
					state["game"]["score0"] = 0;
					state["game"]["team1"] = "ORANGE";
					state["game"]["score1"] = 0;
				}


				

				state["game"]["time"] = server.GetSecondsRemaining(); // game time, counts up if OT
				state["game"]["isOT"] = server.GetbOverTime() ? true : false;
				
				
				BallWrapper ball = server.GetBall();
				if (!ball.IsNull()) {
					state["game"]["ballSpeed"] = static_cast<int>((ball.GetVelocity().magnitude() / 27.778) + 0.5); // Speed in uu/s, 1uu = 1cm, divided by 27.778 to get from cm/s to km/h
					state["game"]["ballTeam"] = ball.GetHitTeamNum();
					state["game"]["isReplay"] = ball.GetbReplayActor() ? true : false;
				}
				else {
					state["game"]["ballSpeed"] = 0;
					state["game"]["ballTeam"] = 255;
					state["game"]["isReplay"] = false;
				}

				TeamWrapper winner = server.GetGameWinner();
				if (!winner.IsNull()) {
					state["game"]["hasWinner"] = true;
					state["game"]["winner"] = winner.GetCustomTeamName().IsNull() ? "" : winner.GetCustomTeamName().ToString();
				}
				else {
					state["game"]["hasWinner"] = false;
					state["game"]["winner"] = "";
				}

				CameraWrapper cam = gameWrapper->GetCamera();
				if (!cam.IsNull()) {
					PriWrapper specPri = PriWrapper(reinterpret_cast<std::uintptr_t>(cam.GetViewTarget().PRI));
					if (!specPri.IsNull()) {
						if (!specPri.IsLocalPlayerPRI()) {
							string name = specPri.GetPlayerName().IsNull() ? "" : specPri.GetPlayerName().ToString();
							state["game"]["hasTarget"] = true;
							state["game"]["target"] = name + "_" + std::to_string(specPri.GetSpectatorShortcut());
						}
						else {
							state["game"]["hasTarget"] = false;
							state["game"]["target"] = "";
						}
					}
					else {
						state["game"]["hasTarget"] = false;
						state["game"]["target"] = "";
					}
				}
				else {
					state["game"]["hasTarget"] = false;
					state["game"]["target"] = "";
				}
			}
		}
	}

	state["hasGame"] = isInGame;

	if (isInGame) {
		isPlaying = true;
		this->SendWsPayload(
			state.dump()
		);

	}
	else {
		if (isPlaying) {
			isPlaying = false;
			this->SendWsPayload(
				state.dump()
			);
		}
	}

	gameWrapper->SetTimeout(bind(&GameStatePlugin::UpdateGameState, this), 0.0500f);
}

void GameStatePlugin::RunWsServer() {
	ws_server = new PluginServer();
	ws_server->init_asio();
	ws_server->set_open_handler(websocketpp::lib::bind(&GameStatePlugin::OnWsOpen, this, _1));
	ws_server->set_close_handler(websocketpp::lib::bind(&GameStatePlugin::OnWsClose, this, _1));
	ws_server->set_message_handler(websocketpp::lib::bind(&GameStatePlugin::OnWsMsg, this, _1, _2));
	//ws_server->set_close_handler(websocketpp::lib::bind(&GameStatePlugin::OnWsClose, this, _1));
	ws_server->set_http_handler(websocketpp::lib::bind(&GameStatePlugin::OnHttpRequest, this, _1));
	ws_server->listen(7599);
	ws_server->start_accept();
	ws_server->run();
}

void GameStatePlugin::OnWsMsg(connection_hdl hdl, PluginServer::message_ptr msg) {
	this->SendWsPayload(
		msg->get_payload()
	);
}

void GameStatePlugin::OnHttpRequest(websocketpp::connection_hdl hdl) {
	PluginServer::connection_ptr connection = ws_server->get_con_from_hdl(hdl);
	connection->append_header("Content-Type", "application/json");
	connection->append_header("Server", "GameStatePlugin/0.1");

	if (connection->get_resource() == "/init") {
		json::JSON data;
		data["event"] = "init";
		data["data"] = "json here";

		connection->set_body(
			data.dump()
		);

		connection->set_status(websocketpp::http::status_code::ok);
		return;
	}

	connection->set_body("Not found");
	connection->set_status(websocketpp::http::status_code::not_found);
}

void GameStatePlugin::SendWsPayload(std::string payload) {
	// broadcast to all connections
	for (connection_hdl it : *ws_connections) {
		ws_server->send(it, payload, websocketpp::frame::opcode::text);
	}
}

void GameStatePlugin::onUnload() {
	// Init websocket server
	if (ws_server != NULL) {
		ws_server->stop();
		ws_server->stop_listening();
	}
	ws_connections->clear();
}