// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Frameworks/RedCGameModeBase.h"
#include "Frameworks/Respawn/RespawnManagerActor.h"
#include "GameModeArena.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRegisterRespawn, AActor*, PlayerStart);

UCLASS()
class PROJECTREDC_API AGameModeArena : public ARedCGameModeBase
{
	GENERATED_BODY()

public:
	AGameModeArena();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = L"") override;
	virtual void Logout(AController* Exiting) override;

	UPROPERTY(EditAnywhere, Category = "ArenaSettings|ClientTravel")
	FString LobbyServerAddress;
	UPROPERTY(EditAnywhere, Category = "ArenaSettings|ClientTravel")
	FString LobbyServerPort;

	UPROPERTY(EditAnywhere, Category = "ArenaSettings|Beacon")
	int32 LobbyBeaconListenPort;


	void HandleEndGame(ETeamType WinnerTeam);
	void ConnectToLobby(ETeamType WinnerTeam);
	void AllPCClientTravelToLobby();
	
	// 디버깅용 서버함수
	void SetLobbyServerAddress(FString IPAddress){LobbyServerAddress = IPAddress;}

private:
	int8 TravelToLobbyCountDown = 5;

#pragma region PlayerRespawn

public:
	// 플레이어를 스폰 시 자동으로 호출되는 함수
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
	// 각 플레이어에게 리스폰 인덱스 발급
	UPROPERTY()
	int32 RedPlayerIndex = 0;
	UPROPERTY()
	int32 BluePlayerIndex = 0;
	
	UPROPERTY()
	TMap<AController*, int32> RedPlayerIdMap;
	UPROPERTY()
	TMap<AController*, int32> BluePlayerIdMap;

	// 레드/블루팀 리스폰 엑터 배열
	UPROPERTY()
	TArray<ARespawnManagerActor*> RedRespawnManager;
	UPROPERTY()
	TArray<ARespawnManagerActor*> BlueRespawnManager;

	UPROPERTY()
	TArray<AActor*> RedRespawnArray;

	UPROPERTY()
	TArray<AActor*> BlueRespawnArray;

	FRegisterRespawn RegisterRespawn;
	
	UFUNCTION(BlueprintCallable)
	FTransform GetRespawnLocation(AActor* DeadChar);
#pragma endregion

public:
	int GetTeamPlayerCount(ETeamType Team) const;
	
private:
	uint8 AssignTeamID = 0;
	UPROPERTY(EditDefaultsOnly)
	int FulledPlayerCount = 4;

};
