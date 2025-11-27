// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/GameMode/GameModeArena.h"

#include "EngineUtils.h"
#include "SocketSubsystem.h"
#include "AI/AICharacter/RedCAICharacter.h"
#include "Characters/RedCHeroCharacter.h"
#include "Controllers/RedCPlayerController.h"
#include "Frameworks/RedCGameStateBase.h"
#include "Frameworks/GameState/GameStateArena.h"
#include "Frameworks/PlayerState/PlayerStateArena.h"
#include "Frameworks/Respawn/RespawnManagerActor.h"
#include "Kismet/GameplayStatics.h"
#include "Online/Beacon/LobbyBeaconClient.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"
#include "Utility/LogHelper.h"
#include "Utility/RedCTravelInfoHelper.h"


AGameModeArena::AGameModeArena()
{
	//bool bCanBind = false;
	//TSharedRef<FInternetAddr> LocalAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBind);
	
	LobbyServerAddress = "0.0.0.0";
	LobbyServerPort = "7777";
	LobbyBeaconListenPort = 15000;
}

void AGameModeArena::BeginPlay()
{
	Super::BeginPlay();

	// 맵에 배치된 RespawnManagerActor 찾기
	TArray<AActor*> FoundManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARespawnManagerActor::StaticClass(), FoundManagers);

	// RespawnManagerActor의 TeamType 분류해서 레드/블루팀 배열에 Add
	for (AActor* Actor: FoundManagers)
	{
		ARespawnManagerActor* Manager = Cast<ARespawnManagerActor>(Actor);
		if (Manager->TeamType == ETeamType::Red)
		{
			RedRespawnManager.Add(Manager);
		}
		else if (Manager->TeamType == ETeamType::Blue)
		{
			BlueRespawnManager.Add(Manager);
		}
	}
}

void AGameModeArena::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AGameStateArena* GS = GetGameState<AGameStateArena>())
	{
		GS->ProjectGameMode = EProjectGameMode::InGameMode;

		// TeamAssign 관련
		GS->AddConnectedPlayer(NewPlayer);
		
		// 플레이어 모두 도착시 게임 시작
		UE_LOG(LogRedCWidget, Warning, TEXT("Player %s, ConnectedPlayers Num : %d"), *NewPlayer->GetName(), GS->GetConnectedPlayers().Num());
		if (GS->GetConnectedPlayers().Num() == FulledPlayerCount)
		{
			GS->GameStart(); //서버용
			GS->IsGameStart = true; //클라용
		}
	}
}

FString AGameModeArena::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	const FString& Options, const FString& Portal)
{
	if (NewPlayerController)
	{
		if (APlayerStateArena* PS = NewPlayerController->GetPlayerState<APlayerStateArena>())
		{
			PS->ParseTravelOptions(Options);

			// if Debug/Editor ? Generate Debug Data : Get by TravelInfo
			if (ARedCPlayerController* PC = Cast<ARedCPlayerController>(NewPlayerController))
			{
				// Make New TravelInfo
				FRedCTravelInfo TravelInfo;
				FGenericTeamId GenericTeamId;
				
				// This Case is In-Editor or Debug
				if (PS->GetPlayerTravelInfo().TeamID.IsEmpty())
				{
					// TeamID Generate
					const uint8 TeamSeed = AssignTeamID % 2;
					UE_LOG(LogRedCArenaInit, Warning, TEXT("AGM-D-001 : Player %s, TeamID is Empty. if you see this message. this game will be Editor or Debug"), *PC->GetName());

					// Set Team ID
					GenericTeamId = FGenericTeamId(TeamSeed);
					UE_LOG(LogRedCArenaInit, Warning, TEXT("AGM-D-002 : Generated Team ID -> %d"), GenericTeamId.GetId());

					// Character Info Generate
					if (TeamSeed == 0)
					{
						TravelInfo.CharacterID = CharacterTags::Character_Warrior.GetTag().ToString();
					}
					else
					{
						TravelInfo.CharacterID = CharacterTags::Character_Archer.GetTag().ToString();
					}
				}
				else // This Case is Real Game
				{
					// TeamID Get by TravelInfo
					UE_LOG(LogRedCArenaInit, Warning, TEXT("AGM-P-001-01 : Player %s, TeamID is %s."), *PC->GetName(), *PS->GetPlayerTravelInfo().TeamID);
					GenericTeamId = FGenericTeamId(FCString::Atoi(*PS->GetPlayerTravelInfo().TeamID));

					// Character Info Get by TravelInfo
					if (PS->GetPlayerTravelInfo().CharacterID == FString("0"))
					{
						TravelInfo.CharacterID = CharacterTags::Character_Warrior.GetTag().ToString();
					}
					else
					{
						TravelInfo.CharacterID = CharacterTags::Character_Archer.GetTag().ToString();
					}

					// Private Player Name
					UE_LOG(LogRedCArenaInit, Warning, TEXT("AGM-P-001-02 : Player %s, Name is %s."), *PC->GetName(), *PS->GetPlayerTravelInfo().PlayerName);
					TravelInfo.PlayerName = PS->GetPlayerTravelInfo().PlayerName;
				}
				PS->SetPlayerTravelInfo(TravelInfo);
				PS->CharacterName = RedCTravelHelper::ConvertCharacterToKorean(TravelInfo.CharacterID);
				UE_LOG(LogRedCArenaInit, Warning, TEXT("AGM-002 : Player %s, CharacterID is %s, CharacterName is %s"), *PC->GetName(), *PS->GetPlayerTravelInfo().CharacterID, *PS->CharacterName.ToString());

				PS->SetPrivatePlayerName(FName(TravelInfo.PlayerName));
				
				PS->SetGenericTeamId(GenericTeamId);
				PS->InitializeCharacterItems(PS->GetPlayerTravelInfo().CharacterID);
				UE_LOG(LogRedCArenaInit, Warning, TEXT("AGM-003 : Player %s, GenericTeamId is %d"), *PC->GetName(), PS->GetGenericTeamId().GetId());
			}
		}
	}

	// Toggle TeamID
	AssignTeamID = (AssignTeamID == 1) ? 0 : 1;
	
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void AGameModeArena::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	//UE_LOG(LogRedCBeacon, Warning, TEXT("Now Arena Server Player Num : %d"), GetNumPlayers()-1);
	
	if (GetNumPlayers() - 1 == 0) 
	{
		//UE_LOG(LogRedCBeacon, Warning, TEXT("Server ResetStart"));
		GetWorld()->ServerTravel("?Restart");
	}
}


#pragma region Travel

void AGameModeArena::HandleEndGame(ETeamType WinnerTeam)
{
	switch (WinnerTeam)
	{
	case ETeamType::Blue:
		{
			// Blue Team Win
			UE_LOG(LogRedCWidget, Warning, TEXT("Blue Team Win"));
			ConnectToLobby(WinnerTeam);	// 전투결과 전달
			break;
		}
	case ETeamType::Red:
		{
			// Red Team Win
			UE_LOG(LogRedCWidget, Warning, TEXT("Red Team Win"));
			ConnectToLobby(WinnerTeam);	// 전투결과 전달
		}
	case ETeamType::Neutral:
		{
			// Error
			UE_LOG(LogRedCWidget, Error, TEXT("MissMatch WinnerTeam / TeamType is Neutral"));
		}
	case ETeamType::None:
		{
			// Warning
			UE_LOG(LogRedCWidget, Warning, TEXT("TimerOver / No One Wine"));
			ConnectToLobby(WinnerTeam);	// 전투결과 전달
		}
	default:
		{
			// Error
			UE_LOG(LogRedCWidget, Error, TEXT("MissMatch WinnerTeam / TeamType is Default"));
		}
	}
	
	// 플레이어 이동
	FTimerHandle TempTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TempTimerHandle,
		this,
		&ThisClass::AllPCClientTravelToLobby,
		TravelToLobbyCountDown,
		false
		);
}


void AGameModeArena::AllPCClientTravelToLobby()
{
	for (ARedCPlayerController* PC : TActorRange<ARedCPlayerController>(GetWorld()))
	{
		if (!PC) continue;

		FString PlayerNameString = FString();
		if (const APlayerStateArena* PS = PC->GetPlayerState<APlayerStateArena>())
		{
			PlayerNameString = PS->GetPrivatePlayerName().ToString();
		}

		PC->ClientTravel(LobbyServerAddress + ":" + LobbyServerPort +
			FString::Format(TEXT("?player={0}"), { PlayerNameString }),
			TRAVEL_Absolute
			);
	}
}

#pragma endregion

#pragma region Beacon

void AGameModeArena::ConnectToLobby(ETeamType WinnerTeam)
{
	// todo = WinnerTeam 정보 넘겨줘야함
	if (ALobbyBeaconClient* LobbyBeaconClient = GetWorld()->SpawnActor<ALobbyBeaconClient>())
	{
		if(!LobbyBeaconClient->ConnectToLobby(LobbyServerAddress, LobbyBeaconListenPort))
		{
			UE_LOG(LogRedCBeacon,Error,TEXT("BeaconClient Failed To Connect To lobby"))
		}
	}
}

#pragma endregion

/********************************************************************************************************/

#pragma region PlayerRespawn
// 플레이어를 스폰 시 자동으로 호출되는 함수
AActor* AGameModeArena::ChoosePlayerStart_Implementation(AController* Player)
{
	if (!Player) return nullptr;
	APlayerStateArena* PS = Player->GetPlayerState<APlayerStateArena>();
	if (!PS) return Super::ChoosePlayerStart_Implementation(Player);
	
	// 플레이어에 리스폰 인덱스 부여
	if (!RedPlayerIdMap.Contains(Player))
	{
		if (PS->GetGenericTeamId() == static_cast<uint8>(ETeamType::Red))
		{
			RedPlayerIdMap.Add(Player, RedPlayerIndex++);
		}
		else if (PS->GetGenericTeamId() == static_cast<uint8>(ETeamType::Blue))
		{
			BluePlayerIdMap.Add(Player, BluePlayerIndex++);
		}
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

FTransform AGameModeArena::GetRespawnLocation(AActor* DeadChar)
{
	if (Cast<ARedCAICharacter>(DeadChar))
	{
		return FTransform();
	}

	if (ARedCHeroCharacter* Player = Cast<ARedCHeroCharacter>(DeadChar))
	{
		// 사망한 캐릭터의 컨트롤러 찾기
		AController* PlayerController = Player->GetController();
		if (!PlayerController) return FTransform();

		// 사망한 캐릭터의 플레이어스테이트 찾기
		APlayerStateArena* PS = Player->GetPlayerState<APlayerStateArena>();

		// 위치 return
		int32 RespawnIndex;
		
		// 레드일 때
		if (PS->GetGenericTeamId() == static_cast<uint8>(ETeamType::Red))
		{
			RespawnIndex = RedPlayerIdMap.Contains(PlayerController) ? RedPlayerIdMap[PlayerController] : -1;
			if (RespawnIndex == -1)
				return FTransform();

			for (AActor* Actor : RedRespawnManager)
			{
				ARespawnManagerActor* Manager = Cast<ARespawnManagerActor>(Actor);
				if (Manager && Manager->PlayerID == RespawnIndex)
				{
					//LogHelper::Print(LogGameModeArena, ELogVerbosity::Warning,
					//FString::Printf(TEXT("Return Location = %s"),*Manager->GetSpawnLocation().ToString()));
			
					return Manager->GetTransform();
				}
			}
		}
		// 블루일 때
		if (PS->GetGenericTeamId() == static_cast<uint8>(ETeamType::Blue))
		{
			RespawnIndex = BluePlayerIdMap.Contains(PlayerController) ? BluePlayerIdMap[PlayerController] : -1;
			if (RespawnIndex == -1)
				return FTransform();

			for (AActor* Actor : BlueRespawnManager)
			{
				ARespawnManagerActor* Manager = Cast<ARespawnManagerActor>(Actor);
				if (Manager && Manager->PlayerID == RespawnIndex)
				{
					//LogHelper::Print(LogGameModeArena, ELogVerbosity::Warning,
					//FString::Printf(TEXT("Return Location = %s"),*Manager->GetSpawnLocation().ToString()));
			
					return Manager->GetTransform();
				}
			}
		}
	}
	return FTransform();
}

int AGameModeArena::GetTeamPlayerCount(ETeamType Team) const
{
	if (Team == ETeamType::Red)
	{
		return RedPlayerIdMap.Num();
	}
	else if (Team == ETeamType::Blue)
	{
		return BluePlayerIdMap.Num();
	}
	return -1;
}

#pragma endregion
