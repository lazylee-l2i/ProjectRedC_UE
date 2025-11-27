// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Spawn/RedCAISpawner.h"

#include "AI/AICharacter/RedCAIDefaultCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Types/RedCLogChannels.h"


ARedCAISpawner::ARedCAISpawner()
{
 	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bNetLoadOnClient = true;
	bAlwaysRelevant = true;

	// 더 필요한 컴포넌트가 있다면 이 아래에 헤더 선언과 함께 초기화 구문 작성.
}

void ARedCAISpawner::BeginPlay()
{
	Super::BeginPlay();

	// 유효성 검사
	if (ValidateAIClassAndWaypoints())
	{
		return;
	}

	// 스포너 초기화 메소드(우선, 풀링만 들어있음)
	InitializeSpawner();
	
	// 활성화 시, 파라미터에 설정한 데로 작동 시작
	OnActivateSpawnerDelegate.AddDynamic(this, &ARedCAISpawner::OnActivateSpawner);

	// 비활성화 시, 스포너 동작 종료
	OnDeactivateSpawnerDelegate.AddDynamic(this, &ARedCAISpawner::OnDeactivateSpawner);

	// AI 캐릭터가 사망 혹은 디버그용 귀환 시, 비활성화
	OnAICharacterDeadDelegate.AddDynamic(this, &ARedCAISpawner::OnAICharacterDead);
	
	
}

// AI Pool을 생성하여 스포너 초기화
void ARedCAISpawner::InitializeSpawner()
{
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	// Create AI Pool
	for (int i = 0; i < PoolSize; i++)
	{
		ARedCAIDefaultCharacter* SpawnedActor = GetWorld()->SpawnActor<ARedCAIDefaultCharacter>(AIClass, PoolTankLocationActor->GetActorLocation(), FRotator::ZeroRotator, SpawnParam);
		SpawnedActor->SetFirstBornLocation(PoolTankLocationActor->GetActorLocation());
		//SpawnedActor->SetActorHiddenInGame(true);
		//SpawnedActor->SetActorEnableCollision(false);
		SpawnedActors.Add(SpawnedActor);
	}
}

ARedCAIDefaultCharacter* ARedCAISpawner::FindAvailableIndexAndGetThatActor()
{
	int32 Index = 0;
	for (auto& AICharacter : SpawnedActors)
	{
		if (not AICharacter->GetIsAlive())
		{
			PoolIndex = Index;
			return AICharacter;
		}
		Index += 1;
	}

	//UE_LOG(LogRedCAI, Warning, TEXT("Not Enough AI Pools."))
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = this;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ARedCAIDefaultCharacter* NewAI = GetWorld()->SpawnActor<ARedCAIDefaultCharacter>(AIClass, PoolTankLocationActor->GetActorLocation(), FRotator::ZeroRotator, SpawnParam);
	NewAI->SetFirstBornLocation(PoolTankLocationActor->GetActorLocation());
	SpawnedActors.Add(NewAI);
	PoolSize += 1;
	return NewAI;
}

// 필드에 소환한다는 개념에 더 가까운 메소드
void ARedCAISpawner::SpawnAI()
{
	//UE_LOG(LogRedCAI, Log, TEXT("Spawner Spawn AI!"));
	if (SpawnedActors.Num() == 0) return;

	//UE_LOG(LogRedCAI, Log, TEXT("Enter SpawnedActors Size Check!"));
	// 지금은 러프하게 작성했지만, 추후 살아있는 AI를 찾는 구문이 있어야함.
	// 지금은 슈도코드로 이해하는게 좋음.
	ARedCAIDefaultCharacter* SpawnedActor = FindAvailableIndexAndGetThatActor();
	//UE_LOG(LogRedCAI, Log, TEXT("Find Remain Actor Name : %s"), *SpawnedActor->GetName());

	SpawnedActor->SetWayPoints(Waypoints);
	//UE_LOG(LogRedCAI, Log, TEXT("Set WayPoints!, WayPoints Num : %d"), Waypoints.Num());
	
	SpawnedActor->SetActorLocation(SpawnLocation + GetActorLocation());
	//UE_LOG(LogRedCAI, Log, TEXT("Set Spawn Location %s!"), *SpawnLocation.ToString());
	
	// 스폰(위치 재설정)됨을 알림.
	SpawnedActor->OnLifeChanged.Broadcast();
	
	// Pool Size만큼 동작하게 
	// PoolIndex = (PoolIndex + 1)%SpawnedActors.Num();
}

// 스포너 On!
void ARedCAISpawner::OnActivateSpawner()
{
	UE_LOG(LogRedCAI, Log, TEXT("Spawner Is Activate!. WaveRepeatInterval = %f"), WaveRepeatInterval);
	// 설정한 스폰 인터벌 시간에 1마리씩 우선 스폰하게 만듦
	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ARedCAISpawner::SpawnAI,
		WaveRepeatInterval,
		true
	);
}

// 스포너 Off!
void ARedCAISpawner::OnDeactivateSpawner()
{
	//UE_LOG(LogRedCAI, Log, TEXT("Spawner Is Deactivate!"));
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
}

// AI 캐릭터 뒈짓
void ARedCAISpawner::OnAICharacterDead(ARedCAIDefaultCharacter* AICharacter)
{
	if (AICharacter)
	{
		AICharacter->OnLifeChanged.Broadcast();
	}
}

TArray<AActor*> ARedCAISpawner::GetWaypoints() const
{
	return Waypoints;
}

bool ARedCAISpawner::ValidateAIClassAndWaypoints() const
{
	bool Validation = false;
	if (Waypoints.Num() == 0)
	{
		//UE_LOG(LogTemp, Error, TEXT("AI Spawner %s : WayPoint is Not Set"), *GetName());
		Validation = false;
	}
	return Validation;
}


