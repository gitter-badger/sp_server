#define HERO_MODE 51
#define INFINITY_SURVIVAL_II_MODE 48
#define INFINITY_KING_MODE 50
#define INFINITY_SYMBOL_MODE 49
#define SURVUVAL_MODE 4
#define KING_SURVIVAL_MODE 15
#define TEAMPLAY_MODE 3
#define CRYSTAL_CAPTURE_MODE 40
#define DUEL_MODE 5
#define LUCKY3_MODE 6
#define ASSAULY_MODE 7
#define GAIN_SYMBOL_MODE 31
#define KING_SLAYER_MODE 9
#define MAGIC_LUCKY3_MODE 10
#define FIGHT_CLUB_MODE 15
#define TOUR_NAMENT_MODE 16
#define BIG_BATTLE_SURVIVAL_MODE 33
#define BIG_MATCH_AUTO_TEAM_SURVIVAL_MODE 34
#define BIG_MATCH_DEATH_MATCH_MODE 35
LobbyList::LobbyList() {
	count = 0;
	root = 0;
}

void LobbyList::Insert(LobbyUser user) {
	count++;
	LobbyUser *Temp = new LobbyUser;
	Temp->name = user.name;
	Temp->gender = user.gender;
	Temp->level = user.level;
	Temp->mission = user.mission;
	Temp->ptr = 0;

	if(!root)
		root = Temp;
	else {
		LobbyUser *p;
		for(p = root; p->ptr; p = p->ptr)
			if(p->name.compare(user.name) == 0) {
				delete Temp;
				return;
			}
		p->ptr = Temp;
	}
}

void LobbyList::Delete(string username) {
	LobbyUser *delPtr, *currPtr;
	if(!root)return;
	count--;
	if(root->name.compare(username) == 0) {
		delPtr = root;
		root = root->ptr;
	} else {
		currPtr = root;
		if(currPtr->ptr == 0)return;
		while(currPtr->ptr->name.compare(username) != 0) {
			currPtr = currPtr->ptr;
			if(currPtr->ptr == 0)return;
		}
		delPtr = currPtr->ptr;
		currPtr->ptr = currPtr->ptr->ptr;
	}
	delete delPtr;
}

int LobbyList::GetList(unsigned char *packet) {
	LobbyUserInfoResponse Lobby_UserInfo_Response;
	memset(&Lobby_UserInfo_Response, 0, sizeof(Lobby_UserInfo_Response));
	Lobby_UserInfo_Response.size = 0x3C;
	Lobby_UserInfo_Response.type = LOBBY_USERINFO_RESPONSE;
	Lobby_UserInfo_Response.unk1 = 11036;
	Lobby_UserInfo_Response.b2 = 1; //1
	Lobby_UserInfo_Response.b5 = 1; //1
	Lobby_UserInfo_Response.b6 = 1; //1
	int i = 0;
	for(LobbyUser *p = root; p; p = p->ptr, i++) {
		Lobby_UserInfo_Response.gender = p->gender;
		strcpy(Lobby_UserInfo_Response.username, p->name.c_str());
		Lobby_UserInfo_Response.level = p->level;
		Lobby_UserInfo_Response.missionlevel = p->mission;
		memcpy(packet + (i * 0x3C), &Lobby_UserInfo_Response, 0x3C);
	}

	return count;
}

struct LobbyRoom {
	int n, p, master, progress, mission;
	bool started;
	char title[29];
	int mode, map, maxp;
	int level[8];
	int life[8];
	bool gender[8];
	string *users[8];
	PacketHandler *Player[8];
	//NpcList npc;
};

class _RoomList {
private:
	LobbyRoom Rooms[22];
public:
	_RoomList() {
		for(int i = 0; i < 22; i++) {
			Rooms[i].n = -1;
			Rooms[i].p = 0;
			Rooms[i].progress = 0;
			Rooms[i].mission = 0;
			Rooms[i].started = false;
			for(int j = 0; j < 8; j++) {
				Rooms[i].level[j] = -99;
				Rooms[i].gender[j] = false;
				Rooms[i].users[j] = 0;
				Rooms[i].Player[j] = 0;
				Rooms[i].life[j] = 0;
				//Rooms[i].npc.size = 0;
				Rooms[i].master = 0;
			}
		}
	}
	sockaddr_in* GetClientUdp(int room, int slot) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == room) {
				if(slot >= 0 && slot < 8)
					if(Rooms[i].Player[slot])
						return Rooms[i].Player[slot]->GetClientUdp();
			}
		return 0;
	}
	void GetUserName(char *name, int room, int slot) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == room) {
				if(slot >= 0 && slot < 8)
					if(Rooms[i].Player[slot])
						strcpy(name, Rooms[i].Player[slot]->Info.usr_name);
			}
	}
	void CreateRoom(LobbyRoom *data) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == -1) {
				Rooms[i].n = data->n;
				strcpy(Rooms[i].title, data->title);
				Rooms[i].mode = data->mode;
				Rooms[i].map = data->map;
				Rooms[i].maxp = data->maxp;
				for(int j = 0; j < 8; j++) {
					Rooms[i].level[j] = data->level[j];
					Rooms[i].gender[j] = data->gender[j];
					Rooms[i].users[j] = data->users[j];
					if(Rooms[i].users[j])Rooms[i].p++;
				}
				break;
			}
	}
	void CreateRoom(PacketHandler *newPlayer, string *s, CreateRoomResponse *CRR, bool gender, int level) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == -1) {
				Rooms[i].n = CRR->roomnumber;
				strcpy(Rooms[i].title, CRR->roomname);
				Rooms[i].mode = CRR->mode;
				Rooms[i].map = CRR->map;
				Rooms[i].maxp = CRR->capacity;
				//for(int j = 0; j < 8; j++)
				//    if(Rooms[i].users[j] == 0)
				//    {
				Rooms[i].level[0] = level;
				Rooms[i].gender[0] = gender;
				Rooms[i].users[0] = s;
				Rooms[i].Player[0] = newPlayer;
				newPlayer->Info.usr_slot = 0;
				Rooms[i].master = 0;
				newPlayer->Info.rm_master = 0;
				newPlayer->Info.usr_mode = Rooms[i].mode;
				Rooms[i].p++;
				Rooms[i].mission = newPlayer->Info.Mission;
				//break;
				//   }
				break;
			}
	}
	bool JoinRoom(PacketHandler *newPlayer, string *s, int n, bool gender, int level) {
		int join = false;
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				for(int j = 0; j < 8; j++)
					if(Rooms[i].users[j] == 0) {
						Rooms[i].level[j] = level;
						Rooms[i].gender[j] = gender;
						Rooms[i].users[j] = s;
						Rooms[i].Player[j] = newPlayer;
						newPlayer->Info.usr_slot = j;
						newPlayer->Info.rm_master = Rooms[i].master;
						newPlayer->Info.usr_mode = Rooms[i].mode;
						Rooms[i].p++;
						join = true;
						break;
					}
				ProdcastInRoomPlayerData(newPlayer, n);
				break;
			}
		return join;
	}

	void UpdateRoomData(LobbyRoom *data) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == data->n) {
				strcpy(Rooms[i].title, data->title);
				Rooms[i].mode = data->mode;
				Rooms[i].map = data->map;
				Rooms[i].maxp = data->maxp;
				break;
			}
	}
	void DeleteRoom(int roomnum) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == roomnum)Rooms[i].n = -1;
	}
	void GetLobbyRoomResponse(int n, LobbyRoomResponse *LRR) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				LRR->unk2 = 1;
				strcpy(LRR->title, Rooms[i].title);
				//LRR->unk3 = 0;
				LRR->mode = Rooms[i].mode;
				LRR->map = Rooms[i].map;
				LRR->unk4 = 0x000;
				LRR->maxplayers = Rooms[i].maxp;
				LRR->unk5 = 0x000;
				LRR->unk6 = 0;
				LRR->unk7 = -1;
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j])LRR->characters[j] = Rooms[i].Player[j]->Info.usr_char;
					else LRR->characters[j] = 0;
					for(int i = 0; i < 13; i++)LRR->zeros[i] = 0;
					LRR->unk9 = 0x100;
			}
	}
	void GetRoomList(RoomListResponse* RLR) {
		int x = 0;
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n != -1) {
				RLR->roomnumber[x] = Rooms[i].n;
				strcpy(RLR->title[x], Rooms[i].title);
				RLR->mode[x] = Rooms[i].mode;
				RLR->map[x] = Rooms[i].map;
				RLR->maxplayers[x] = Rooms[i].maxp;
				RLR->unks2[x] = 1;
				RLR->unks4[x] = -1;
				for(int j = 0; j < Rooms[i].maxp - 1; j++) {
					RLR->players[x][j] = Rooms[i].Player[j] ? Rooms[i].Player[j]->Info.usr_char : 0;
				}
				x++;
			}
		while(x < 22) {
			RLR->roomnumber[x] = -1;
			x++;
		}
	}
	void GetRoomPlayerList(int n, RoomPlayerListResponse *RPLR) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				RPLR->master = Rooms[i].master;
				for(int j = 0; j < 8; j++)
					if(Rooms[i].users[j]) {
						RPLR->plevel[j] = Rooms[i].level[j];
						RPLR->genders[j] = Rooms[i].gender[j];
						strcpy(RPLR->username[j], Rooms[i].users[j]->c_str());
					} else RPLR->plevel[j] = -99;

					break;
			}
	}
	int GetInRoomPlayerList(int n, unsigned char *pack) {
		int count = 0;
		RoomPlayerDataResponse Room_PlayerData_Response;
		memset(&Room_PlayerData_Response, 0, sizeof(RoomPlayerDataResponse));
		Room_PlayerData_Response.size = 0x118;
		Room_PlayerData_Response.type = ROOM_PLAYERDATA_RESPONSE;
		Room_PlayerData_Response.unk1 = 11036;
		Room_PlayerData_Response.unk9 = 0x01000000; //0x01000000
		Room_PlayerData_Response.unk20 = -1; //-1
		Room_PlayerData_Response.unk21 = -1; //-1
		//Room_PlayerData_Response.unk22 = 0x01010000;//0x01010000
		Room_PlayerData_Response.unk26 = -1; //-1
		Room_PlayerData_Response.team = 1;
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				//if(Rooms[i].mode >= 12 && Rooms[i].mode <= 27)
					 //Room_PlayerData_Response.team = 0xA; //0xA
				Room_PlayerData_Response.mission = Rooms[i].mission;
				if(CheckReady(n))  Rooms[i].started = true;
				else Rooms[i].started = false;

				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j]) {
						Room_PlayerData_Response.Slot = j;
						Rooms[i].Player[j]->GetInRoomData(&Room_PlayerData_Response, Rooms[i].started);
						memcpy(pack + (0x118 * count), &Room_PlayerData_Response, 0x118);
						count++;
						ProdcastInRoomPlayerData(Rooms[i].Player[j], n);
					}
				break;
			}
		return count;
	}
	void ProdcastChangeTitle(RoomTitleChangeRequest *R) {
		RoomTitleChangeResponse RTCR;
		RTCR.size = 0x34;
		RTCR.type = CHANGE_ROOMTITLE_RESPONSE;
		RTCR.unk1 = 11036;
		for(int i = 0; i < 32; i++)RTCR.title[i] = 0;
		strcpy(RTCR.title, R->title);
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == R->room) {
				strcpy(Rooms[i].title, RTCR.title);
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j])Rooms[i].Player[j]->GetTitleChange(&RTCR);
			}

	}
	void ProdcastInRoomPlayerData(PacketHandler *newPlayer, int n) {
		RoomPlayerDataResponse Room_PlayerData_Response;
		memset(&Room_PlayerData_Response, 0, sizeof(RoomPlayerDataResponse));
		Room_PlayerData_Response.size = 0x118;
		Room_PlayerData_Response.type = ROOM_PLAYERDATA_RESPONSE;
		Room_PlayerData_Response.unk1 = 11036;
		Room_PlayerData_Response.unk9 = 0x01000000; //0x01000000
		//Room_PlayerData_Response.unk19 = 2; //2
		//Room_PlayerData_Response.team = 1;
		Room_PlayerData_Response.unk20 = -1; //-1
		Room_PlayerData_Response.unk21 = -1; //-1
		//Room_PlayerData_Response.unk22 = 0x01010000; //0x01010000
		Room_PlayerData_Response.unk26 = -1; //-1
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				Room_PlayerData_Response.mission = Rooms[i].mission;
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j] == newPlayer) {
						//if(Rooms[i].mode >= 12 && Rooms[i].mode <= 27)
							 //Room_PlayerData_Response.team = 0xA;

						if(CheckReady(n))  Rooms[i].started = true;
						else Rooms[i].started = false;
						Room_PlayerData_Response.Slot = j;
						Rooms[i].Player[j]->GetInRoomData(&Room_PlayerData_Response, Rooms[i].started);
						for(int x = 0; x < 8; x++)
							if(Rooms[i].Player[x] && Rooms[i].Player[x] != Rooms[i].Player[j])
								Rooms[i].Player[x]->GetInRoomPlayerList(&Room_PlayerData_Response);
						break;
					}

				break;
			}
	}
	bool CheckReady(int n) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				int mode = Rooms[i].mode;
				int Quest_Mode_Ready[15] = {11,18,23,12,19,24,13,20,25,14,21,26,16,22,27};
				for(int j = 0; j < 15; j++) {
					if(Quest_Mode_Ready[j] == Rooms[i].mode) {
						if(Rooms[i].started) return true;
						for(int k = 0; k < Rooms[i].p; k++) {
							if(!Rooms[i].Player[k]->Info.usr_ready) return false;
						}
						return true;
					}
				}
				switch(mode) {
				case BIG_BATTLE_SURVIVAL_MODE:
				case BIG_MATCH_AUTO_TEAM_SURVIVAL_MODE:
				case BIG_MATCH_DEATH_MATCH_MODE:
					return true;
					break;
				case CRYSTAL_CAPTURE_MODE:
				{
					if(Rooms[i].started)
						return true;
					int TEAM1 = 0, TEAM2 = 0;
					for(int j = 0; j < Rooms[i].p; j++) {
						if(Rooms[i].Player[j]->Info.usr_team == 10) TEAM1++;
						else TEAM2++;
					}
					if(TEAM1 == 0 || TEAM2 == 0)
						return false;
					for(int j = 0; j < Rooms[i].p; j++) {
						if(!Rooms[i].Player[j]->Info.usr_ready)
							return false;
					}
					return true;
				}
				break;
				case TEAMPLAY_MODE:
				{
					int TEAM1 = 0, TEAM2 = 0;
					for(int j = 0; j < Rooms[i].p; j++) {
						if(Rooms[i].Player[j]->Info.usr_team == 10) TEAM1++;
						else TEAM2++;
					}
					if(TEAM1 == TEAM2) {
						for(int j = 0; j < Rooms[i].p; j++) {
							if(!Rooms[i].Player[j]->Info.usr_ready)
								return false;
						}
						return true;
					} else return false;
				}
				break;
				case DUEL_MODE:
				{
					int TWO_READY = 0;
					for(int j = 0; j < 2; j++) {
						if(Rooms[i].Player[j])
							if(Rooms[i].Player[j]->Info.usr_ready) TWO_READY++;
					}
					if(TWO_READY == 2)
						return true;
					else
						return false;
					break;
				}
				default:
					for(int j = 0; j < Rooms[i].p; j++) {
						if(!Rooms[i].Player[j]->Info.usr_ready)
							return false;
					}
					return true;
					break;
				}
			}
	}
	bool IsAllReady(int n) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j])
						if(Rooms[i].Player[j]->Info.usr_ready == 0)return false;
			}
		return true;
	}
	int CheckRound(int roomnum, int deadslot) {
		for(int i = 0; i < Rooms[roomnum].p; i++) {
			if(Rooms[roomnum].Player[i]->Info.usr_team == Rooms[roomnum].Player[deadslot]->Info.usr_team) {
				if(Rooms[roomnum].life[deadslot] < Rooms[roomnum].life[i])
					return false;
			}
		}
		return true;
	}
	void ProdcastPlayerExitRoom(PacketHandler *player, RoomExitResponse *RER, int n) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j] == player) {
						for(int x = 0; x < 8; x++) {
							if(Rooms[i].Player[x] && Rooms[i].Player[x] != Rooms[i].Player[j]) {
								Rooms[i].Player[x]->GetPlayerExitRoomResponse(RER);
								if(Rooms[i].master == j) {
									Rooms[i].master = x;
									for(int y = 0; y < 8; y++)
										if(Rooms[i].Player[y])
											Rooms[i].Player[y]->GetMasterResponse(x);
								}
							}
						}
						break;
					}
			}
	}
	void ProdcastInRoomUpgrade(PacketHandler *newPlayer, CardUpgradeResponse *CUR, int n) {
		CUR->unk5 = newPlayer->Info.usr_slot;
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j] == newPlayer) {
						for(int x = 0; x < 8; x++)
							if(Rooms[i].Player[x] && Rooms[i].Player[x] != Rooms[i].Player[j])
								Rooms[i].Player[x]->GetInRoomUpgradeResponse(CUR);
						break;
					}
			}
	}
	bool CheckLife(int roomnum, int mode, int deathslot) {
		int Quest_Mode_Life[15] = {11,18,23,12,19,24,13,20,25,14,21,26,16,22,27};
		for(int i = 0; i < 15; i++) {
			if(Quest_Mode_Life[i] == mode) {
				for(int j = 0; j < Rooms[roomnum].p; j++)
					if(Rooms[roomnum].life[j] > 0)return true;
				return false;
			}
		}
		switch(mode) {
		case TEAMPLAY_MODE:
			for(int i = 0; i < Rooms[roomnum].p; i++) {
				if(Rooms[roomnum].Player[i]->Info.usr_team == Rooms[roomnum].Player[deathslot]->Info.usr_team) {
					if(Rooms[roomnum].life[i] >= 0) {
						return true;
					}
				}
			}
			return false;
			break;
		case 5:
			if(Rooms[roomnum].life[deathslot] >= 0) return true;
			else return false;
			break;
		default:
			for(int i = 0; i < Rooms[roomnum].p; i++)
				if(Rooms[roomnum].life[i] > 0) return true;
			return false;
			break;
		}
	}
	int CheckResult(int mode, int killerteam, int slotteam) {
		switch(mode) {
		case TEAMPLAY_MODE:
			if(killerteam == slotteam) return 1;
			else return 2;
			break;
		case DUEL_MODE:
			if(killerteam == slotteam) return 1;
			else return 2;
			break;
		}

	}
	void ProdcastDeathResponse(PlayerKilledRequest *PK, int n) {
		cout << "-- PlayerKilledRequest --" << endl;
		PlayerKilledResponse PKR;
		PKR.size = 0xAC;
		PKR.type = PLAYER_KILLED_RESPONSE;
		PKR.unk1 = 11036;
		PKR.DeadplayerSlot = PK->deathplayerslot;
		PKR.KillerSlot = PK->killerslot;
		PKR.PointsMiltiplier = 5;
		PKR.unk2 = 3; //3,2
		PKR.unk3 = 1; //1
		PKR.unk4 = 1; //1
		PKR.unk5 = 1; //1
		PKR.unk6 = 1; //1
		PKR.unk7 = 1; //1
		PKR.unk8 = 1; //1
		for(int i = 0; i < 6; i++)PKR.zeros[i] = 0;
		for(int i = 0; i < 16; i++)PKR.unks[i] = -1;
		PKR.unk01 = 2; //maybe kills
		PKR.unk02 = 1; //0,1 maybe king
		PKR.unk03 = 1; //1
		//PKR.canRespawn = Info.quest; //0
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n  && Rooms[i].p >= 1) {
				int KillerTeam = Rooms[i].Player[PK->killerslot]->Info.usr_team;
				int SlotTeam = 0;
				if(PKR.DeadplayerSlot >= 0 && PKR.DeadplayerSlot < 8)Rooms[i].life[PKR.DeadplayerSlot]--;
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j]) {
						PKR.canRespawn = Rooms[i].life[j];
						Rooms[i].Player[j]->GetInRoomDeathResponse(&PKR);
					}
				cout << "-- CheckLife --" << endl;
				if(CheckLife(i, Rooms[i].mode, PK->deathplayerslot) == false) {
					cout << "Game Over" << endl;
					ResultsResponse Results_Response;
					memset(&Results_Response, 0, sizeof(Results_Response));
					Results_Response.size = 0x90;
					Results_Response.type = RESULTS_RESPONSE;
					Results_Response.unk1 = 11036;
					for(int j = 0; j < Rooms[i].p; j++) {
						SlotTeam = Rooms[i].Player[j]->Info.usr_team;
						if(Rooms[i].Player[j])Results_Response.result[j] = CheckResult(Rooms[i].mode, KillerTeam, SlotTeam);
					}
					for(int j = 0; j < Rooms[i].p; j++)
						if(Rooms[i].Player[j])
							Rooms[i].Player[j]->GetResultResponse(&Results_Response);
				} else {
					cout << "There is life" << endl;
					if(CheckRound(i, PK->deathplayerslot)) {
						cout << "Round Over" << endl;
						ResultsResponse Results_Response;
						memset(&Results_Response, 0, sizeof(Results_Response));
						Results_Response.size = 0x90;
						Results_Response.type = RESULTS_RESPONSE;
						Results_Response.unk1 = 11036;
						Results_Response.unk2 = 0;
						Results_Response.unk3 = 0;
						for(int j = 0; j < 8; j++)
							if(Rooms[i].Player[j])Results_Response.result[j] = 0;
						for(int j = 0; j < 8; j++)
							if(Rooms[i].Player[j])
								Rooms[i].Player[j]->GetResultResponse(&Results_Response);
					}
				}
			}
	}
	void ProdcastRoomJoinResponse2(PacketHandler *player, int n) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j])
						if(Rooms[i].Player[j] != player)Rooms[i].Player[j]->GetJoinResponse2();
			}
	}
	void ProdcastNpcList(PacketHandler *pMaster, NpcList *npc, int n) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j])
						if(Rooms[i].Player[j] != pMaster)Rooms[i].Player[j]->GetNpcList(npc);
			}
	}
	void ProdcastExpGain(QuestGainResponse *packet, int n) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j])
						Rooms[i].Player[j]->GetExpGainResponse(packet);
			}
	}
	void ProdcastReviveResponse(ReviveResponse *Revive_Response, int n) {
		Revive_Response->type = REVIVE_RESPONSE;
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				if(Rooms[i].Player[Revive_Response->Slot]) {
					for(int j = 0; j < 8; j++)
						if(Rooms[i].Player[j])
							Rooms[i].Player[j]->GetReviveResponse(Revive_Response);
				}
			}

	}
	void ProdcastKickResponse(int n, int slot) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				int x = Rooms[i].master;
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j]) {
						Rooms[i].Player[j]->GetKickResponse(slot);
						if(x == Rooms[i].master)x = j;
					}
				if(Rooms[i].master == slot) {
					Rooms[i].master = x;
					for(int y = 0; y < 8; y++)
						if(Rooms[i].Player[y])
							Rooms[i].Player[y]->GetMasterResponse(x);
				}
			}
	}
	void SendTradeResponse(TradeStruct *TS, int n) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				for(int j = 0; j < 8; j++)
					if(Rooms[i].users[j]) {
						if(!strcmp((char*)Rooms[i].users[j]->c_str(), TS->username2))
							Rooms[i].Player[j]->GetTradeResponse(TS);
					}
			}
	}
	void GetRoomData(RoomJoinResponse *RJR) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == RJR->roomnumber) {
				strcpy(RJR->title, Rooms[i].title);
				RJR->mode = Rooms[i].mode;
				RJR->map = Rooms[i].map;
				RJR->maxp = Rooms[i].maxp;
				break;
			}
	}
	void UpdateProgress(int n, int p) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				Rooms[i].progress = p;
				if(p >= 100) {
					ResultsResponse Results_Response;
					memset(&Results_Response, 0, sizeof(Results_Response));
					Results_Response.size = 0x90;
					Results_Response.type = RESULTS_RESPONSE;
					Results_Response.unk1 = 11036;
					for(int j = 0; j < 8; j++)
						if(Rooms[i].Player[j])Results_Response.result[j] = 1;
					for(int j = 0; j < 8; j++)
						if(Rooms[i].Player[j])
							Rooms[i].Player[j]->GetResultResponse(&Results_Response);
				}
				break;
			}
	}
	bool ExitPlayer(int n, PacketHandler *player) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j] == player) {
						Rooms[i].users[j] = 0;
						player->Info.usr_ready = 0;
						player->Info.usr_slot = -1;
						player->Info.usr_char = player->Info.DefaultCharacter;
						Rooms[i].Player[j] = 0;
						Rooms[i].level[j] = -99;
						Rooms[i].gender[j] = 0;
						Rooms[i].p--;
						break;
					}
				if(!Rooms[i].p) {
					Rooms[i].n = -1;
					Rooms[i].master = 0;
					//Rooms[i].npc.size = 0;
					return true;
				}
				break;
			}
		return false;
	}
	void InitializeLife(int n) {
		for(int i = 0; i < 22; i++)
			if(Rooms[i].n == n) {
				for(int j = 0; j < 8; j++)
					if(Rooms[i].Player[j]) {
						if(Rooms[i].Player[j]->Info.questlife)
							Rooms[i].life[j] = 2;
						else Rooms[i].life[j] = 1;
					}
			}
	}

}RoomList;

struct UserHandle {
	PacketHandler *Handler;
	UserHandle *ptr;
};

class _HandleList {
private:
	int count;
	UserHandle *root;

public:
	_HandleList() {
		count = 0;
		root = 0;
	}
	void Insert(PacketHandler* pHandler) {
		count++;
		UserHandle *loc = new UserHandle;
		loc->Handler = pHandler;
		UserHandle *prevPtr = 0;
		UserHandle *currPtr = root;
		while(currPtr != 0) {
			if(strcmp(pHandler->Info.usr_name, currPtr->Handler->Info.usr_name) > 0)break;
			prevPtr = currPtr;
			currPtr = currPtr->ptr;
		}
		loc->ptr = currPtr;
		if(prevPtr == 0)
			root = loc;
		else prevPtr->ptr = loc;
	}
	void Delete(PacketHandler* pHandler) {
		UserHandle *Temp;
		if(!root)return;
		count--;
		UserHandle *delPtr, *currPtr;
		if(!root)return;
		count--;
		if(root->Handler == pHandler) {
			delPtr = root;
			root = root->ptr;
		} else {
			currPtr = root;
			while(currPtr->ptr->Handler != pHandler)
				currPtr = currPtr->ptr;
			delPtr = currPtr->ptr;
			currPtr->ptr = currPtr->ptr->ptr;
		}
		delete delPtr;
	}
	int updateUdpState(char *s) {
		UserHandle *currPtr = root;
		while(currPtr != NULL) {
			if(strcmp(s, currPtr->Handler->Info.usr_name) == 0)break;
			currPtr = currPtr->ptr;
		}
		if(currPtr)
			return currPtr->Handler->UpdateUDPState();
		else {
			cout << "Unable to update " << s << "state : UDPLIST" << endl;
			return 0;
		}
	}
	void updateUdpClient(char *s, sockaddr_in client) {
		UserHandle *currPtr = root;
		while(currPtr != NULL) {
			if(strcmp(s, currPtr->Handler->Info.usr_name) == 0)break;
			currPtr = currPtr->ptr;
		}
		if(currPtr)
			currPtr->Handler->UpdateUDPClient(client);
		else
			cout << "Unable to update " << s << "client : UDPLIST" << endl;
	}
	void ProdcastChat(PacketHandler* pHandler, ChatRequest *Chat_Request) {
		ChatResponse Chat_Response;
		memset(&Chat_Response, 0, sizeof(Chat_Response));
		Chat_Response.size = 0x7C;
		Chat_Response.type = CHAT_RESPONSE;
		Chat_Response.unk1 = 11036;
		Chat_Response.chatType = Chat_Request->chatType;
		strcpy(Chat_Response.senderId, Chat_Request->senderId);
		strcpy(Chat_Response.msg, Chat_Request->msg);
		for(UserHandle *p = root; p; p = p->ptr)
			if(pHandler != p->Handler)p->Handler->GetChatMessage(&Chat_Response);
	}
	void ProdcastLobbyInfo(PacketHandler* pHandler, LobbyUser* LobbyInfo, bool b) {
		LobbyUserInfoResponse Lobby_UserInfo_Response;
		memset(&Lobby_UserInfo_Response, 0, sizeof(Lobby_UserInfo_Response));
		Lobby_UserInfo_Response.size = 0x3C;
		Lobby_UserInfo_Response.type = LOBBY_USERINFO_RESPONSE;
		Lobby_UserInfo_Response.unk1 = 11036;
		Lobby_UserInfo_Response.gender = LobbyInfo->gender;
		Lobby_UserInfo_Response.b2 = b; //1
		Lobby_UserInfo_Response.b5 = b; //1
		Lobby_UserInfo_Response.b6 = b; //1
		strcpy(Lobby_UserInfo_Response.username, LobbyInfo->name.c_str());
		Lobby_UserInfo_Response.level = LobbyInfo->level;
		Lobby_UserInfo_Response.missionlevel = LobbyInfo->mission;
		for(UserHandle *p = root; p; p = p->ptr)
			if(pHandler != p->Handler)p->Handler->GetLobbyMessage(&Lobby_UserInfo_Response);
	}
	void ProdcastNewRoom(PacketHandler* pHandler, CreateRoomResponse *CRR, bool b, int roomnum = 0) {
		LobbyRoomResponse LRR;
		memset(&LRR, 0, sizeof(LRR));
		LRR.size = 0xB0;
		LRR.type = NEW_ROOM_RESPONSE;
		LRR.unk1 = 11036;
		if(b) {
			LRR.unk2 = 1;
			LRR.roomnumber = CRR->roomnumber;
			strcpy(LRR.title, CRR->roomname);
			LRR.mode = CRR->mode; //0x1b
			LRR.map = CRR->map; //0x1b
			LRR.unk4 = 0x000; //0x100
			LRR.maxplayers = CRR->capacity; //8
			LRR.unk5 = 0x000;
			LRR.unk7 = -1; //-1
			LRR.characters[0] = pHandler->Info.usr_char; //0x46
			for(int i = 1; i < 8; i++)
				LRR.characters[i] = 0;
			for(int i = 1; i < 13; i++)
				LRR.zeros[i] = 0;
			LRR.unk9 = 0x100;
		} else {
			LRR.unk2 = 0;
			LRR.roomnumber = roomnum;
			LRR.mode = -1;
			LRR.map = -1;
			LRR.unk7 = -1;
		}

		for(UserHandle *p = root; p; p = p->ptr)
			if(pHandler != p->Handler)p->Handler->GetNewRoomMessage(&LRR);

	}
	void ProdcastRoomUpdate(int room) {
		LobbyRoomResponse LRR;
		LRR.size = 0xB0;
		LRR.type = NEW_ROOM_RESPONSE;
		LRR.unk1 = 11036;
		LRR.roomnumber = room;
		RoomList.GetLobbyRoomResponse(room, &LRR);
		UserHandle *p;
		for(p = root; p; p = p->ptr) {
			if(p->Handler->Info.usr_room == -1)
				p->Handler->GetNewRoomMessage(&LRR);
		}
	}

} HandleList;




