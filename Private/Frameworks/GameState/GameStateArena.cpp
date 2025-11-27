// Fill out your copyright notice in the Description page of Project Settings.


#include "Frameworks/GameState/GameStateArena.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AI/Spawn/RedCAISpawnActor.h"
#include "Controllers/RedCPlayerController.h"
#include "Frameworks/GameMode/GameModeArena.h"
#include "GameFramework/GameMode.h"
#include "GAS/Effects/GiveCoinEffect.h"
#include "Net/UnrealNetwork.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"


void AGameStateArena::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGameStateArena, IsGameStart);
	DOREPLIFETIME(AGameStateArena, BlueTowerCount);
	DOREPLIFETIME(AGameStateArena, RedTowerCount);
}


void AGameStateArena::AddConnectedPlayer(APlayerController* PlayerController)
{
	if (PlayerController == nullptr) return;
	ConnectedPlayers.Add(PlayerController);
}


void AGameStateArena::OnRep_IsGameStart()
{
	if (IsGameStart)
	{
		OnStartGamePlay.Broadcast();
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(GamePlayTimeTimerHandle);
	}
}


void AGameStateArena::OnRep_BlueTowerCount()
{
	OnBlueTeamTowerDestroyed.Broadcast();
}

void AGameStateArena::OnRep_RedTowerCount()
{
	OnRedTeamTowerDestroyed.Broadcast();
}

// Server
void AGameStateArena::HandleDestroyTower(AActor* DestroyedTower)
{
	if (ARedCAI_Tower* Tower = Cast<ARedCAI_Tower>(DestroyedTower))
	{
		int8 TeamID =  Tower->GetGenericTeamId();
		switch (TeamID)
		{
			case 0:
			{
				BlueTeamTowers.Remove(Tower);
				BlueTowerCount = BlueTeamTowers.Num();
				Tower->OnTowerDestroyed.RemoveDynamic(this, &AGameStateArena::HandleDestroyTower);
					UE_LOG(LogRedC,Warning,TEXT("Blue Tower destroyed"));
				break;
			}
			case 1:
			{
				RedTeamTowers.Remove(Tower);
				RedTowerCount = RedTeamTowers.Num();
				Tower->OnTowerDestroyed.RemoveDynamic(this, &AGameStateArena::HandleDestroyTower);
					UE_LOG(LogRedC,Warning,TEXT("Red Tower destroyed"));
				break;
			}
			default:
			{
				UE_LOG(LogRedC, Warning, TEXT("TeamID Error! / None"));
				break;
			}
		}
	}

	CheckAllTowerDestroyed();
}

//Server
void AGameStateArena::HandleGameEndUI(ETeamType WinnerTeam)
{
	int8 TeamID = GetWinnerTeamInt(WinnerTeam);
	for (APlayerController* BasePC : ConnectedPlayers)
	{
		if (ARedCPlayerController* PC = Cast<ARedCPlayerController>(BasePC))
		{
			if (APlayerStateArena* PS = PC->GetPlayerState<APlayerStateArena>())
			{
				if (TeamID == 2)
				{
					//draw
					PC->Client_ShowGameDrawUI();
				}
				else if (PS->GetGenericTeamId().GetId()== TeamID)
				{
					//win
					PC->Client_ShowGameWinnerUI();
				}
				else
				{
					//lose
					PC->Client_ShowGameLoserUI();
				}
			}
		}
	}
}

// Server
void AGameStateArena::CheckAllTowerDestroyed()
{
	if (IsGameEnd) return; 
	if (AGameModeArena* GM =GetWorld()->GetAuthGameMode<AGameModeArena>())
	{
		if (BlueTeamTowers.Num() == 0) //Red Team Win
		{
			HandleGameEndUI(ETeamType::Red); //엔딩 위젯 띄우기
			GM->HandleEndGame(ETeamType::Red);
			IsGameEnd = true;
		}
		else if (RedTeamTowers.Num() == 0) //Blue Team Win
		{
			HandleGameEndUI(ETeamType::Blue); //엔딩 위젯 띄우기
			GM->HandleEndGame(ETeamType::Blue);
			IsGameEnd = true;
		}
	}
}

int8 AGameStateArena::GetWinnerTeamInt(ETeamType Team)
{
	switch (Team)
	{
		case ETeamType::Red:
		{
			return 1;
		}
		case ETeamType::Blue:
		{
			return 0;
		}
		case ETeamType::None:
		{
			return 2;
		}
		default:
		return -1;
	}
}


void AGameStateArena::AddBlueTeamTowerArray(ARedCAI_Tower* BlueTeamTower)
{
	if (BlueTeamTower == nullptr) return;
	
	BlueTeamTowers.Add(BlueTeamTower);
	BlueTowerCount = BlueTeamTowers.Num();
	BlueTeamTower->OnTowerDestroyed.AddDynamic(this, &AGameStateArena::HandleDestroyTower);
	UE_LOG(LogRedC, Warning, TEXT("BlueTeamTower Add!"));
}

void AGameStateArena::AddRedTeamTowerArray(ARedCAI_Tower* RedTeamTower)
{
	if (RedTeamTower == nullptr) return;
	
	RedTeamTowers.Add(RedTeamTower);
	RedTowerCount = RedTeamTowers.Num();
	RedTeamTower->OnTowerDestroyed.AddDynamic(this, &AGameStateArena::HandleDestroyTower);
	UE_LOG(LogRedC, Warning, TEXT("RedTeamTower Add!"));
}

void AGameStateArena::AddStarterWall(ABaseStarterWall* Wall)
{
	if (Wall == nullptr) return;
	StarterWalls.Add(Wall);
}

void AGameStateArena::AddAISpawners(ARedCAISpawnActor* AISpawn)
{
	if (IsValid(AISpawn))
	{
		AISpawners.Add(AISpawn);
	}
}


int32 AGameStateArena::GetRemainGamePlayTime() const
{
	return RemainGamePlayTime;
}

TArray<APlayerController*> AGameStateArena::GetConnectedPlayers()
{
	return ConnectedPlayers;
}

TArray<ARedCAI_Tower*> AGameStateArena::GetRedTeamTowers()
{
	return RedTeamTowers;
}

TArray<ARedCAI_Tower*> AGameStateArena::GetBlueTeamTowers()
{
	return BlueTeamTowers;
}

int AGameStateArena::GetBlueTeamTowerNum()
{
	return BlueTowerCount;
}

int AGameStateArena::GetRedTeamTowerNum()
{
	return RedTowerCount;
}

TArray<ABaseStarterWall*> AGameStateArena::GetStarterWalls()
{
	return StarterWalls;
}


// Server
void AGameStateArena::GameStart()
{
	// 시작지점 방어벽 제거
	for (ABaseStarterWall* k : StarterWalls)
	{
		k->Destroy(true);
	}

	for (ARedCAISpawnActor* Spawner : AISpawners)
	{
		Spawner->ToggleSpawner();
	}
	
	// 게임타이머 시작
	GetWorld()->GetTimerManager().SetTimer(
		GamePlayTimeTimerHandle,
		this,
		&AGameStateArena::UpdateGamePlayTime,
		1.0f,
		true
	);


	if (HasAuthority())
	{
		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TempHandle,
			this,
			&AGameStateArena::GiveGlobalTimeCoin,
			5.f,
			true
		);
	}
}

void AGameStateArena::UpdateGamePlayTime()
{
	if (RemainGamePlayTime <= 0) // 게임타이머 종료
	{
		GetWorld()->GetTimerManager().ClearTimer(GamePlayTimeTimerHandle);
		IsGameStart =false;
		
		if (IsGameEnd) return; 
		if (AGameModeArena* GM = GetWorld()->GetAuthGameMode<AGameModeArena>())
		{
			HandleGameEndUI(ETeamType::None);
			GM->HandleEndGame(ETeamType::None);
			IsGameEnd = true;
		}
		return;
	}
	else
	{
		RemainGamePlayTime--;
	}
}

void AGameStateArena::GiveCoinHandle(AActor* GivenActor, AActor* DeadActor)
{
	EActorType ActorType = ICharacterIdentifier::GetActorType(DeadActor);
	switch (ActorType)
	{
		case EActorType::Hero:
		case EActorType::Minion:
			GivePersonalCoin(GivenActor, DeadActor, ActorType);
			break;
		case EActorType::Tower:
			GiveTeamCoin(ICharacterIdentifier::GetCharacterTeam(GivenActor), 0);
			break;
		default:
			break;
	}
}

void AGameStateArena::GivePersonalCoin(AActor* GivenActor, AActor* DeadActor, EActorType ActorType)
{
	if (AGameModeArena* GM = GetWorld()->GetAuthGameMode<AGameModeArena>())
	{
		float CoinValue = 150.f;
		if (ActorType == EActorType::Hero)
		{
			APlayerStateArena* GS = nullptr;
			APlayerStateArena* DS = nullptr;
			if (ACharacter* Given = Cast<ACharacter>(GivenActor))
			{
				GS = Given->GetPlayerState<APlayerStateArena>();
			}
			if (ACharacter* Dead = Cast<ACharacter>(DeadActor))
			{
				DS = Dead->GetPlayerState<APlayerStateArena>();
			}
			if (GS && DS)
			{
				int LevelDiff = FMath::Max(DS->CharacterLevel - GS->CharacterLevel, 0);
				CoinValue = 150.f + LevelDiff * 150.f;
			}
		}
		
		ApplyCoinEffect(GivenActor, CoinValue);
	}
}

void AGameStateArena::GiveTeamCoin(ETeamType TeamId, float CoinAmount)
{
	if (AGameModeArena* GM = GetWorld()->GetAuthGameMode<AGameModeArena>())
	{
		float CoinValue = CoinAmount == 0.f ? 500.f : CoinAmount;
		TMap<AController*, int> PlayerMap;
		if (ETeamType::Red == TeamId)
		{
			PlayerMap = GM->RedPlayerIdMap;
			
		}
		else if (ETeamType::Blue == TeamId)
		{
			PlayerMap = GM->BluePlayerIdMap;
		}

		for (TPair<AController*, int> Pair : PlayerMap)
		{
			ApplyCoinEffect(Pair.Key->GetPawn(), CoinValue);
		}
	}
}

FGameplayEffectSpecHandle AGameStateArena::GenerateCoinEffectHandle(AActor* GivenActor)
{
	if (UAbilitySystemComponent* GivenASC = TryGetActorASCInMap(GivenActor))
	{
		FGameplayEffectContextHandle CoinEffectContext = GivenASC->MakeEffectContext();
		CoinEffectContext.AddSourceObject(GivenActor);
		

		FGameplayEffectSpecHandle CoinSpecHandle = GivenASC->MakeOutgoingSpec(
			UGiveCoinEffect::StaticClass(),
			1.0f,
			CoinEffectContext
		);

		if (CoinSpecHandle.IsValid())
		{
			return CoinSpecHandle;
		}
	}
	return nullptr;
}

void AGameStateArena::GiveGlobalTimeCoin()
{
	GiveTeamCoin(ETeamType::Red, 25.f);
	GiveTeamCoin(ETeamType::Blue, 25.f);
}


void AGameStateArena::ApplyCoinEffect(AActor* GivenActor, float CoinAmount)
{
	if (not IsValid(GivenActor))
	{
		return;
	}
	
	FGameplayEffectSpecHandle CoinSpecHandle = GenerateCoinEffectHandle(GivenActor);
	if (CoinSpecHandle.IsValid())
	{
		CoinSpecHandle.Data->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_Coin, CoinAmount);
		if (UAbilitySystemComponent* ASC = TryGetActorASCInMap(GivenActor))
		{
			ASC->ApplyGameplayEffectSpecToSelf(*CoinSpecHandle.Data.Get());
		}
	}
}

UAbilitySystemComponent* AGameStateArena::TryGetActorASCInMap(AActor* Actor) const
{
	if (CachedASCMap.Find(Actor))
	{
		return CachedASCMap.FindRef(Actor);
	}
	else
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
		{
			CachedASCMap.Add(Actor, ASC);
			return ASC;
		}
		return nullptr;
	}
}
