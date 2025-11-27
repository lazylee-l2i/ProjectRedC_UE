// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/Notify/AN_ProjectileSpawnEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/RedCHeroCharacter.h"
#include "Characters/Components/CrosshairComponent.h"
#include "GAS/RedCAbilitySystemComponent.h"
#include "Interface/RedCAbilitySystemInterface.h"
#include "Projectiles/RedCProjectile.h"


void UAN_ProjectileSpawnEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!NotifyValidationCheck(MeshComp))
	{
		return;
	}

	check(ProjectileClass);

	AActor* Actor = MeshComp->GetOwner();
	if (!Actor)
	{
		return;
	}

	// Ability를 실행한 플레이어인 경우에만 동작
	ARedCHeroCharacter* HeroCharacter = Cast<ARedCHeroCharacter>(Actor);
	if (HeroCharacter && !HeroCharacter->IsLocallyControlled())
	{
		return;
	}

	// ASC가 유효해야만 Projectile Spawn Event 발행 가능
	URedCAbilitySystemComponent* ASC = Cast<URedCAbilitySystemComponent>(HeroCharacter->GetAbilitySystemComponent());
	if (!ASC)
	{
		return;
	}

	// 플레이어의 캐릭터일 경우에만 CrosshairComponent 사용
	UCrosshairComponent* CrosshairComponent = HeroCharacter ? IRedCAbilitySystemInterface::Execute_GetCrosshairComponent(HeroCharacter) : nullptr;

	// Projectile 위치, 방향, 회전, 속도, 시간 설정
	const FVector SpawnLocation = MeshComp->GetSocketLocation(SocketName);
	const FVector Direction = CrosshairComponent ? CrosshairComponent->GetCrosshairDirection(SpawnLocation, Distance) : Actor->GetActorForwardVector();
	const FRotator Rotation = Direction.Rotation();
	const FVector TargetLocation = SpawnLocation + Direction * Distance;
	const float Time = (TargetLocation - SpawnLocation).Size() / Speed;

	// Projectile 속성 값 설정
	FRedCProjectileInfo ProjectileInfo;
	ProjectileInfo.ProjectileClass = ProjectileClass;
	ProjectileInfo.StartLocation = SpawnLocation;
	ProjectileInfo.Direction = Direction;
	ProjectileInfo.Rotation = Rotation;
	ProjectileInfo.EndLocation = TargetLocation;
	ProjectileInfo.Speed = Speed;
	ProjectileInfo.Distance = Distance;
	ProjectileInfo.Time = Time;

	// HitReactMessage 구조체 값 설정
	ProjectileInfo.HitReactMessage.GameplayCueTag = GameplayCueTag;
	ProjectileInfo.HitReactMessage.HitStrengthTag = HitStrengthTag;
	ProjectileInfo.HitReactMessage.DamageAmount = Damage;
	ProjectileInfo.HitReactMessage.DamageMultiplier = DamageMultiplier;
	ProjectileInfo.HitReactMessage.KnockbackDistance = KnockbackDistance;
	ProjectileInfo.HitReactMessage.KnockDownHeight = LaunchHeight;
	ProjectileInfo.HitReactMessage.StaggerDuration = StaggerDuration;
	ProjectileInfo.HitReactMessage.MaximumAttackLength = Distance * 1.5f;
	
	// SendProjectileSpawnEvent 함수 호출
	ASC->SendProjectileSpawnEvent(EventTag, ProjectileInfo);
}
