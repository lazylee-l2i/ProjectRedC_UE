// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Spawn/RedCAISpawnActor.h"

#include "TimerManager.h"
#include "AI/AIController/RedCAIController.h"
#include "Frameworks/GameState/GameStateArena.h"
#include "Types/RedCLogChannels.h"

// Sets default values
ARedCAISpawnActor::ARedCAISpawnActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ARedCAISpawnActor::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return; 

	// AI 풀 초기화
	InitializeAIPool();

	// 웨이브 반복 시작
	CurrentWaveIndex = 0;

	//ToggleSpawner();  스포너 바로 시작하고 싶으면 키기
	if (AGameStateArena* GS = GetWorld()->GetGameState<AGameStateArena>())
	{
		GS->AddAISpawners(this);
	}
}

void ARedCAISpawnActor::InitializeAIPool()
{
	for (auto& Wave : Waves)
	{
		if (!Wave.AIClass) continue;

		TArray<ARedCAICharacter*>& Pool = AI_Pool.FindOrAdd(Wave.AIClass);	

		//지정된 풀사이즈 만큼 만들기
		for (int32 i = 0; i < PoolSize; i++)
		{
			if (ARedCAICharacter* NewAI = CreateNewPooledAI(Wave.AIClass, Wave.TeamID))
			{
				//생성한 ai 풀에 등록
				Pool.Add(NewAI);
			}
		}
	}
}

void ARedCAISpawnActor::SpawnWave()
{
	if (SpawnPointsActors.Num() == 0 || Waves.Num() == 0)
	{
		return;
	}

	if (!Waves.IsValidIndex(CurrentWaveIndex))
	{
		CurrentWaveIndex = 0;
	}

	const FAISpawn& Wave = Waves[CurrentWaveIndex];

	for (int32 i = 0; i < Wave.Count; i++)
	{
		// 배열 순환으로 SpawnPoint 선택
		ARedC_AISpawnPoint* SpawnPointActor = SpawnPointsActors[i % SpawnPointsActors.Num()];
		if (!SpawnPointActor) continue;

		const FVector SpawnLoc = SpawnPointActor->GetActorLocation();
	
		FTimerDelegate TimerDel;
	
		TimerDel.BindUObject(
			this,
			&ARedCAISpawnActor::SpawnAI,
			Wave.AIClass,
			SpawnLoc,
			Wave.TeamID
			);
	
		const float Delay = FMath::Max(Wave.SpawnDelay * i, 0.01f);
	
		FTimerHandle LocalHandle; 
		GetWorld()->GetTimerManager().SetTimer(
			LocalHandle,
			TimerDel,
			Delay,
			false);
	}
	CurrentWaveIndex = (CurrentWaveIndex + 1) % Waves.Num();
	
}

void ARedCAISpawnActor::SpawnAI(TSubclassOf<ARedCAICharacter> AIClass, FVector SpawnLocation, int32 TeamID)
{
	if (!HasAuthority()) return;  

	FAISpawn* Wave = Waves.FindByPredicate([AIClass](const FAISpawn& Wave){ return Wave.AIClass == AIClass; });

	if (!Wave || Wave->Waypoints.Num() == 0)return;

	ARedCAICharacter* AIChar = GetPooledAI(AIClass, TeamID);
	if (!AIChar) return;
	
	AIChar->OwnerSpawnerWeak = this;
	AIChar->SetActorLocation(SpawnLocation);
	AIChar->Revive(Wave->Waypoints);  
	
}

ARedCAICharacter* ARedCAISpawnActor::GetPooledAI(TSubclassOf<ARedCAICharacter> AIClass, int32 TeamID)
{
	TArray<ARedCAICharacter*>* PoolPtr = AI_Pool.Find(AIClass);
	if (!PoolPtr) return nullptr;
	
	// 숨겨진 AI 중 활성화 안된 것 가져오기
	for (ARedCAICharacter* AIChar : *PoolPtr)
	{
		if (AIChar && !AIChar->bIsActiveInWorld) // 단순 체크
		{
			if (AAIController* Ctrl = Cast<AAIController>(AIChar->GetController()))
			{
				Ctrl->SetGenericTeamId(FGenericTeamId(static_cast<uint8>(TeamID)));
			}
			return AIChar;  //재사용
		}
	}

	// 없으면 새로 생성하고 반환
	ARedCAICharacter * NewAI= CreateNewPooledAI( AIClass,TeamID);
	if (NewAI)
	{
		PoolPtr->Add(NewAI);
	}
	return NewAI;
}

void ARedCAISpawnActor::ReturnAIToPool(ARedCAICharacter* AIChar) // 죽은거 ai풀로 가져오기
{
	if (!AIChar) return;
	
	// 안전하게 비활성화
	AIChar->DeactivateAI();
	AIChar->OnAIDeath.Clear();
	//UE_LOG(LogRedC, Warning, TEXT("[Spawner] %s returned to pool"), *AIChar->GetName());
	
}

ARedCAICharacter* ARedCAISpawnActor::CreateNewPooledAI(TSubclassOf<ARedCAICharacter> AIClass, int32 TeamID)
{
	// 풀 가득차면 새로스폰;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 			
	ARedCAICharacter* NewAIChar = GetWorld()->SpawnActor<ARedCAICharacter>
	(AIClass,
	FVector(0.f, 0.f, -5000.f),
	FRotator::ZeroRotator,
	SpawnParams);
 			
	if (NewAIChar)
	{
		NewAIChar->SetReplicates(true);
		NewAIChar->SetReplicateMovement(true);
		NewAIChar->DeactivateAI();
	}
	return NewAIChar;
}


void ARedCAISpawnActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(CleanupTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(WaveRepeatTimerHandle);
}


void ARedCAISpawnActor::ToggleSpawner()
{
	if (!HasAuthority()) return;  

	if (GetWorldTimerManager().IsTimerActive(WaveRepeatTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(WaveRepeatTimerHandle);
	}
	else
	{
		GetWorldTimerManager().SetTimer(
		  WaveRepeatTimerHandle,
		  this,
		  &ARedCAISpawnActor::SpawnWave,
		  WaveRepeatInterval,
		  true,   // 반복
		  0.0f    // 첫웨이브 실행 기다리는 시간   0이면 즉시스폰
	  );
	}
}

