// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Frameworks/RedCGameModeBase.h"
#include "GameModeLobby.generated.h"

class ARedCPlayerController;

/**
 * 
 */
UCLASS()
class PROJECTREDC_API AGameModeLobby : public ARedCGameModeBase
{
	GENERATED_BODY()
	
public:
	AGameModeLobby();
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = L"") override;

	// 매칭 플레이어 관리
	bool RegisterPlayerController(ARedCPlayerController* PC);
	FTimerHandle ResetGSStateTimerHandle;
	void PendingMatchPlayerControllers_Clear();

	// 캐릭터 선택창
	void ShowCharacterSelectionUI();

	// 아레나 레벨 종료 핸들러
	void LobbyReceivedEndGame();

	// Getter
	int32 GetTargetPlayers() const {return TargetPlayerCount;};

	// 디버깅용 서버함수
	void SetMatchServerAddress(FString IPAddress){MatchServerAddress = IPAddress;}

private:
	// 매칭된 플레이어 트레블
	void MatchedPlayerTravelToArenaServer();
	
	// 매칭된 플레이어 컨트롤러 리스트
	UPROPERTY()
	TArray<TWeakObjectPtr<ARedCPlayerController>> PendingMatchPlayerControllers;

	// 매치 메이킹 카운트
	UPROPERTY()
	int32 MatchmakingCount;

	// 매칭 플레이어 카운트
	UPROPERTY(EditAnywhere, Category = "LobbySettings|ClientTravel")
	int32 TargetPlayerCount;

	// 매칭 서버 주소
	UPROPERTY(EditAnywhere, Category = "LobbySettings|ClientTravel")
	FString MatchServerAddress;
	UPROPERTY(EditAnywhere, Category = "LobbySettings|ClientTravel")
	FString MatchServerPort;

	// 로비 리슨 포트
	UPROPERTY(EditAnywhere, Category = "LobbySettings|Beacon")
	int32 LobbyListenPort;

	
};
