// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/GameMode/GameModeLobby.h"

#include "SocketSubsystem.h"
#include "Controllers/RedCPlayerController.h"
#include "Frameworks/GameState/GameStateLobby.h"
#include "Frameworks/PlayerState/PlayerStateLobby.h"
#include "Online/Beacon/LobbyBeaconHost.h"
#include "Online/Beacon/LobbyBeaconHostObject.h"
#include "Types/RedCLogChannels.h"
#include "UI/Manager/UIManagerSubsystem.h"


AGameModeLobby::AGameModeLobby()
	: MatchmakingCount(0), TargetPlayerCount(0), LobbyListenPort(0)
{
	//bool bCanBind = false;
	//TSharedRef<FInternetAddr> LocalAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBind);

	MatchServerPort = "7780";
	MatchServerAddress = "0.0.0.0";
}

void AGameModeLobby::BeginPlay()
{
	Super::BeginPlay();
	if (GetNetMode() == NM_DedicatedServer)
	{
		ALobbyBeaconHost* Host = GetWorld()->SpawnActor<ALobbyBeaconHost>();
		if (Host && Host->InitHost())
		{
			ALobbyBeaconHostObject* HostObj = GetWorld()->SpawnActor<ALobbyBeaconHostObject>();
			if (HostObj)
			{
				Host->RegisterHost(HostObj);
				UE_LOG(LogTemp, Log, TEXT("[Lobby] Beacon Host ready (port from INI)"));
			}
		}
	}
}

void AGameModeLobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (AGameStateLobby* GS = GetGameState<AGameStateLobby>())
	{
		GS->ProjectGameMode = EProjectGameMode::LobbyMode;
	}
}

FString AGameModeLobby::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	const FString& Options, const FString& Portal)
{
	if (NewPlayerController)
	{
		if (APlayerStateLobby* PS = NewPlayerController->GetPlayerState<APlayerStateLobby>())
		{
			PS->ParseTravelOptions(Options);

			FRedCTravelInfo TravelInfo;
			TravelInfo.PlayerName = PS->GetPlayerTravelInfo().PlayerName;

			PS->SetPlayerTravelInfo(TravelInfo);
		}
	}
	
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

bool AGameModeLobby::RegisterPlayerController(ARedCPlayerController* PC)
{
	if (!PC) return false;

	// 중복 체크
	for (const TWeakObjectPtr<ARedCPlayerController>& It : PendingMatchPlayerControllers)
	{
		if (It.Get() == PC) return false;
	}

	PendingMatchPlayerControllers.Add(PC);

	if (AGameStateLobby* GS = GetGameState<AGameStateLobby>())
	{
		GS->PressedCount = PendingMatchPlayerControllers.Num();
	}

	// 매칭된 플레이어 수 = 매칭 플레이어 카운트 시 매칭된 플레이어 컨트롤러 모두 클라이언트 트레블
	if (PendingMatchPlayerControllers.Num() == TargetPlayerCount)
	{
		if (AGameStateLobby* GS = GetWorld()->GetGameState<AGameStateLobby>())
		{
			// 게임 매칭완료
			for (int32 i = PendingMatchPlayerControllers.Num() - 1; i >= 0; i--)
			{
				if (ARedCPlayerController* TravelPlayerController = PendingMatchPlayerControllers[i].Get())
				{
					if (APlayerStateLobby* PS = TravelPlayerController->GetPlayerState<APlayerStateLobby>())
					{
						PS->MatchingCompleted = true;
					}
				}
			}
			GS->MakeTeam(PendingMatchPlayerControllers);
			// 매칭시작 / 다른플레어들 매칭제한 
			GS->MatchStarted = true;
			// 로비 - 카운트다운 시작
			GS->StartLobbyCountDown();
			// 로비 - 카운트다운 완료 수신
			GS->OnMatchingCountDownFinished.AddUObject(this, &ThisClass::ShowCharacterSelectionUI);
		}
	}

	return true;
}

void AGameModeLobby::ShowCharacterSelectionUI()
{
	for (int32 i = PendingMatchPlayerControllers.Num() - 1; i >= 0; i--)
	{
		if (ARedCPlayerController* TravelPlayerController = PendingMatchPlayerControllers[i].Get())
		{
			TravelPlayerController->Client_ShowCharacterSelectionUI();
		}
	}
	if (AGameStateLobby* GS = GetWorld()->GetGameState<AGameStateLobby>())
	{
		// 캐릭터선택창 - 카운트다운 시작
		GS->StartCharacterSelectionCountDown();
		// 캐릭터선택창 - 카운트다운 완료 수신
		GS->OnMatchingCharacterSelectionCountDownFinished.AddUObject(this, &ThisClass::MatchedPlayerTravelToArenaServer);
	}
}

void AGameModeLobby::MatchedPlayerTravelToArenaServer()
{
	for (int32 i = PendingMatchPlayerControllers.Num() - 1; i >= 0; i--)
	{
		if (ARedCPlayerController* PC = PendingMatchPlayerControllers[i].Get())
		{
			PC->Client_StartLoadingWidget();
			if (APlayerStateLobby* PS = PC->GetPlayerState<APlayerStateLobby>())
			{
				FString MatchID = FString::Printf(TEXT("MatchID : %d"), MatchmakingCount);
				FString TravelOptions = PS->BuildTravelOptions(MatchID, PS->TeamIDForLobby);
				
				UE_LOG(LogRedC, Warning, TEXT("Travel to Arena Server : %s"), *TravelOptions);
				PC->ClientTravel(MatchServerAddress  + ":" + MatchServerPort + TravelOptions, TRAVEL_Absolute);
				PS->MatchingCompleted = false;
				MatchmakingCount++;
			}
		}
	}

	// 5초뒤 Match 관련 Lobby Game State 초기화
	GetWorld()->GetTimerManager().SetTimer(
		ResetGSStateTimerHandle,
		this,
		&ThisClass::PendingMatchPlayerControllers_Clear,
		3,
		false
	);
}

void AGameModeLobby::PendingMatchPlayerControllers_Clear()
{
	if (AGameStateLobby* GS = GetWorld()->GetGameState<AGameStateLobby>())
	{
		GS->MatchStarted = false;
		GS->PressedCount = 0;
		GS->MatchingCharacterSelectionCountDown = GS->ResetCharacterSelectionCountDown;
	}
	PendingMatchPlayerControllers.Empty();
}


void AGameModeLobby::LobbyReceivedEndGame()
{
	UE_LOG(LogRedCBeacon, Warning, TEXT("Lobby Received GameEnd"));
}
