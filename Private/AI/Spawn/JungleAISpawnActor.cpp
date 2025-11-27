// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Spawn/JungleAISpawnActor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AIController.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"

AJungleAISpawnActor::AJungleAISpawnActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void AJungleAISpawnActor::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	InitializeAIPool();

	// 맵에서 SpawnPoint Actor 수집
	for (TActorIterator<ARedC_JungleMobSpawnPoint> It(GetWorld()); It; ++It)
	{
		JungleSpawnPoints.Add(*It);

		FTimerHandle TimerHandle;
		int32 TeamID;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, SpawnPoint = *It, TeamID]()
	   {
		   SpawnAtPoint(SpawnPoint, TeamID);
	   }), It->RespawnTime, true);

		SpawnTimerHandles.Add(TimerHandle);
	}

}

void AJungleAISpawnActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Spawn 타이머 정리
	for (auto& Handle : SpawnTimerHandles)
	{
		GetWorld()->GetTimerManager().ClearTimer(Handle);
	}
	SpawnTimerHandles.Empty();
}

void AJungleAISpawnActor::SpawnAtPoint(ARedC_JungleMobSpawnPoint* SpawnPoint, int32 TeamID)
{
	if (!HasAuthority() || !SpawnPoint || !SpawnPoint->MobClass) return;

	// 이미 존재하는 몹이면 스폰 안함
	if (SpawnPoint->CurrentMob && !SpawnPoint->CurrentMob->IsHidden()) return;

	// MobClass가 ARedCAICharacter를 상속하는지 체크
	if (!SpawnPoint->MobClass->IsChildOf(ARedCAICharacter::StaticClass())) return;

	// 안전하게 TSubclassOf<ARedCAICharacter> 생성
	TSubclassOf<ARedCAICharacter> MobClass(SpawnPoint->MobClass);

	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	ARedCAICharacter* Mob = GetPooledAI(MobClass, TeamID);
	
	if (!Mob) 
	{
	  Mob = GetWorld()->SpawnActor<ARedCAICharacter>(
	SpawnPoint->MobClass, // 포인트마다 다른 몹 클래스 사용
	SpawnPoint->GetActorLocation(),
	SpawnPoint->GetActorRotation(),
	SpawnParams

	);
} 
	if (!Mob) return;
	
	Mob->SetActorLocation(SpawnPoint->GetActorLocation());
	Mob->SetActorHiddenInGame(false);
	Mob->SetActorEnableCollision(true);
	Mob->SetActorTickEnabled(true);

	SpawnPoint->CurrentMob = Mob;
	Mob->TeamID = SpawnPoint->TeamID;
	
	// 필요시 AIController 초기화, BT 시작 가능
	if (AAIController* AICon = Cast<AAIController>(Mob->GetController()))
	{
		// BT 시작 등 처리
	}
	UE_LOG(LogTemp, Display, TEXT("JungleAISpawnActor: Spawned Mob %s at %s and %d and %f"),
		*SpawnPoint->MobClass->GetName(),
		*SpawnPoint->GetActorLocation().ToString(),
		 SpawnPoint->TeamID,
	     SpawnPoint->RespawnTime);
}
	

// 나중에 몹 만들때 사망시 처리부분에
// if (OwningSpawnPoint)
// {
// 	OwningSpawnPoint->CurrentMob = nullptr;
// }
//이런거 넣어놔야 초기화됨