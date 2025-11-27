// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/RedCProjectile.h"

#include "AbilitySystemComponent.h"
#include "AI/AICharacter/RedCAI_Tower.h"
#include "Characters/RedCCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Types/RedCGameplayTags.h"


ARedCProjectile::ARedCProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	NetUpdateFrequency = 100.f;
	MinNetUpdateFrequency = 66.f;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProjectileMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileMesh->SetIsReplicated(true);
	SetRootComponent(ProjectileMesh);

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision"));
	ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileCollision->SetupAttachment(GetRootComponent());

	WorldCollision = CreateDefaultSubobject<USphereComponent>(TEXT("WorldCollision"));
	WorldCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WorldCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	WorldCollision->SetupAttachment(GetRootComponent());

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

void ARedCProjectile::InitializeProjectile(const FRedCProjectileInfo& ProjectileInfo)
{
	HitReactMessage = ProjectileInfo.HitReactMessage;
	
	if (IsValid(ProjectileMovement))
	{
		ProjectileMovement->InitialSpeed = ProjectileInfo.Speed;
		ProjectileMovement->ProjectileGravityScale = 0.f;
	}
	
	SetLifeSpan(ProjectileInfo.Time);
}

void ARedCProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		HitReactMessage.AttackDirection = GetActorForwardVector();
		
		ProjectileCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);
		WorldCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);
	}
}

void ARedCProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	if (CheckSameTeam(OtherActor))
	{
		return;
	}
	
	if (ARedCCharacter* Caster = Cast<ARedCCharacter>(GetInstigator()))
	{
		HitReactMessage.WeaponType = WeaponType;
		HitReactMessage.ImpactLocation = GetActorLocation();
		HitReactMessage.AttackDirection = GetInstigator()->GetActorRotation().Vector();
		
		if (ARedCCharacter* OtherCharacter = Cast<ARedCCharacter>(OtherActor))
		{
			const UAbilitySystemComponent* ASC = OtherCharacter->GetAbilitySystemComponent();
			if (!ASC ||
			ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Invincible) ||
			ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death))
			{
				return;
			}
			
			Caster->ServerReceiveHitReactData(HitReactMessage, OtherCharacter);
		}
		else if (ARedCAI_Tower* Tower = Cast<ARedCAI_Tower>(OtherActor))
		{
			Caster->GetCombatComponent()->ServerApplyDamageToTarget(HitReactMessage, Tower);
		}
	}
	
	Destroy();
}

bool ARedCProjectile::CheckSameTeam(AActor* OtherActor)
{
	if (ICharacterIdentifier* OtherCharacter = Cast<ICharacterIdentifier>(OtherActor))
	{
		if (ICharacterIdentifier* Caster = Cast<ICharacterIdentifier>(GetInstigator()))
		{
			return Caster->IsCharacterSameTeam(OtherCharacter);
		}
	}
	
	return false;
}
