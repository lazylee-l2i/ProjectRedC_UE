// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/CombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/RedCCharacter.h"
#include "GAS/Effects/ApplyDamageEffect.h"
#include "GAS/Effects/EncounterEffect.h"
#include "Types/RedCGameplayTags.h"
#include "Types/RedCLogChannels.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCombatComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (not GetOwner())
		return;

	if(GetOwner()->HasAuthority())
	{
		if (not HitReactMessages.IsEmpty())
		{
			ProcessHitMessage();
		}
		else
		{
			SetComponentTickEnabled(false);
		}
	}
}

#pragma region Cache Actor ASC
void UCombatComponent::SetActorASCInMap(AActor* Actor) const
{
	if (IsValid(Actor))
	{
		if (GetActorASCInMap(Actor))
		{
			return;
		}
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
		{
			CachedASCMap.Add(Actor, ASC);	
		}
	}
}

UAbilitySystemComponent* UCombatComponent::GetActorASCInMap(AActor* Actor) const
{
	if (IsValid(Actor) && CachedASCMap.Find(Actor))
	{
		return CachedASCMap.FindRef(Actor);
	}
	return nullptr;
}

// 이 메소드를 사용하여 Actor의 ASC를 얻을 때, ASC가 없다면
// ASC를 얻는 시도를 추가로 진행함.
UAbilitySystemComponent* UCombatComponent::TryGetActorASCInMap(AActor* Actor) const
{
	if (not IsValid(Actor))
	{
		return nullptr;
	}
	
	if (UAbilitySystemComponent* ASC = GetActorASCInMap(Actor))
	{
		return ASC;
	}
	else
	{
		SetActorASCInMap(Actor);
		return GetActorASCInMap(Actor);
	}
}

#pragma endregion

#pragma region Hit Reaction Data Processing
void UCombatComponent::PushHitMessage(FHitReactMessage& Message)
{
	if (not IsValid(GetOwner()))
	{
		return;
	}
	if (not GetActorASCInMap(GetOwner()))
	{
		SetActorASCInMap(GetOwner());
	}
	
	if (UAbilitySystemComponent* ASC = GetActorASCInMap(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Death))
		{
			UE_LOG(LogRedC, Warning, TEXT("Player is Death. Ignore HitReactMessage"));
			return;
		}
		if (ASC->HasMatchingGameplayTag(PlayerStateTags::Player_State_Combat_KnockedDown) && 
				Message.HitStrengthTag == SharedTags::Event_Shared_HitReact_Light.GetTag())
		{
			Message.HitStrengthTag = SharedTags::Event_Shared_HitReact_Heavy.GetTag();
		}
		Message.bInit = true;

		HitReactMessages.Enqueue(Message);
		++CachedHitReactMessageCount;
		SetComponentTickEnabled(true);
	}
	
}

FHitReactMessage UCombatComponent::PopHitMessage()
{
	if (not HitReactMessages.IsEmpty())
	{
		FHitReactMessage Message;
		HitReactMessages.Dequeue(Message);
		--CachedHitReactMessageCount;
		return Message;
	}
	return FHitReactMessage();
}

void UCombatComponent::ProcessHitMessage()
{
	if (ARedCCharacter* Character = Cast<ARedCCharacter>(GetOwner()))
	{
		FHitReactMessage Message;
		HitReactMessages.Peek(Message);
		FGameplayTag HitStrengthTag = Message.HitStrengthTag;
		if (HitStrengthTag.MatchesTag(SharedTags::Event_Shared_HitReact_Heavy))
		{
			FRotator Rotation = Message.AttackDirection.Rotation() + FRotator(0.0f, 180.0f, 0.0f);
			Character->UnlockControllerRotation();
			Character->ForceSetCharacterRotation(Rotation);
			Character->SwitchHitBoxStateToKnockdown();	
		}
		Character->ProcessHitReaction(HitStrengthTag);
	}
}

void UCombatComponent::ClearHitMessageContainer()
{
	HitReactMessages.Empty();
}

void UCombatComponent::ApplyHitDilationUsingMontage(AActor* DilationActor, float DilationTime) const
{
	if (ACharacter* Character = Cast<ACharacter>(DilationActor))
	{
		if (Character->GetMesh())
		{
			auto AnimInst = Character->GetMesh()->GetAnimInstance();
			if (AnimInst)
			{
				UAnimMontage* CurrentMontage = AnimInst->GetCurrentActiveMontage();
				AnimInst->Montage_SetPlayRate(CurrentMontage, 0.01f);
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					TimerHandle,
					[AnimInst, CurrentMontage]()
					{
						AnimInst->Montage_SetPlayRate(CurrentMontage, 1.0f);
					},
					DilationTime,
					false
				);
			}
		}
	}
}

#pragma endregion

#pragma region Apply Damage & Generate Effect Data

void UCombatComponent::ServerApplyDamageToOwner_Implementation(const FHitReactMessage& Message, AActor* Attacker)
{
	if (not IsPlayersHasASC(Attacker, GetOwner()))
	{
		return;	
	}
	UAbilitySystemComponent* OwnerASC = TryGetActorASCInMap(GetOwner());
	UAbilitySystemComponent* AttackerASC = TryGetActorASCInMap(Attacker);

	if (not OwnerASC || not AttackerASC)
	{
		//UE_LOG(LogRedCServer, Warning, TEXT("OwnerASC or AttackerASC is not valid. %s"), *GetNameSafe(this));
		return;
	}

	FGameplayEffectSpecHandle DamageEffectSpec = GenerateDamageEffectHandle(Message, Attacker, GetOwner());
	
	if (DamageEffectSpec.IsValid())
	{
		//UE_LOG(LogRedCServer, Warning, TEXT("Damage Effect Spec Generation is Complete. %s"), *GetNameSafe(this));
		OwnerASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpec.Data.Get());
	}
	
	FGameplayEffectSpecHandle OwnerEffect = GenerateEncounterEffectHandle(GetOwner());
	FGameplayEffectSpecHandle AttackerEffect = GenerateEncounterEffectHandle(Attacker);
	if (OwnerEffect.IsValid() && AttackerEffect.IsValid())
	{
		OwnerASC->ApplyGameplayEffectSpecToSelf(*OwnerEffect.Data.Get());
		AttackerASC->ApplyGameplayEffectSpecToSelf(*AttackerEffect.Data.Get());

		// Owner에게 Hit React Sound 적용
		if (Message.HitStrengthTag.MatchesTag(FGameplayTag::RequestGameplayTag("Event.Shared.HitReact")))
		{
			FGameplayCueParameters CueParameters;
			FGameplayEffectContextHandle EffectContextHandle = OwnerASC->MakeEffectContext();

			EffectContextHandle.AddInstigator(GetOwner(), GetOwner());
			EffectContextHandle.AddOrigin(Message.ImpactLocation);
			
			if (FCustomGameplayEffectContext* CustomEffectContext = static_cast<FCustomGameplayEffectContext*>(EffectContextHandle.Get()))
			{
				CustomEffectContext->SetWeaponType(Message.WeaponType);
			}

			CueParameters.EffectContext = EffectContextHandle;
			OwnerASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Shared.HitReact"), CueParameters);
		}

		// Owner에게 CameraShake 적용
		FGameplayCueParameters CueParams;
		FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
		EffectContext.AddInstigator(GetOwner(), GetOwner());
		EffectContext.AddSourceObject(OwnerASC);
    
		CueParams.EffectContext = EffectContext;
		CueParams.NormalizedMagnitude = Message.HitStrengthTag.MatchesTag(SharedTags::Event_Shared_HitReact_Heavy) ? 0.7f : 0.5f;
    
		OwnerASC->ExecuteGameplayCue(GameplayCueTags::GameplayCue_Shared_CameraShake, CueParams);
    
		// Target에게 CameraShake 적용
		FGameplayCueParameters TargetCueParams;
		FGameplayEffectContextHandle TargetEffectContext = AttackerASC->MakeEffectContext();
		TargetEffectContext.AddInstigator(Attacker, Attacker);
		TargetEffectContext.AddSourceObject(AttackerASC);
    
		TargetCueParams.EffectContext = TargetEffectContext;
		TargetCueParams.NormalizedMagnitude = Message.HitStrengthTag.MatchesTag(SharedTags::Event_Shared_HitReact_Heavy) ? 0.7f : 0.5f;
    
		AttackerASC->ExecuteGameplayCue(GameplayCueTags::GameplayCue_Shared_CameraShake, TargetCueParams);
	}
}

void UCombatComponent::ServerApplyDamageToTarget_Implementation(const FHitReactMessage& Message, AActor* Target) const
{
	if (not IsValid(Target) && not IsPlayersHasASC(GetOwner(), Target))
	{
		return;
	}

	UAbilitySystemComponent* OwnerASC = TryGetActorASCInMap(GetOwner());
	UAbilitySystemComponent* TargetASC = TryGetActorASCInMap(Target);
	if (not OwnerASC || not TargetASC)
	{
		UE_LOG(LogRedCServer, Warning, TEXT("UCombatComponent::ApplyDamageToTarget  OwnerASC or TargetASC is not valid. %s"), *GetNameSafe(this));
		return;
	}

	FGameplayEffectSpecHandle DamageEffectSpecHandle = GenerateDamageEffectHandle(Message, GetOwner(), Target);
	if (DamageEffectSpecHandle.IsValid())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());	
	}
	
	FGameplayEffectSpecHandle EncounterEffectSpecHandle = GenerateEncounterEffectHandle(GetOwner());
	FGameplayEffectSpecHandle TargetEffectSpecHandle = GenerateEncounterEffectHandle(Target);
	if (EncounterEffectSpecHandle.IsValid() && TargetEffectSpecHandle.IsValid())
	{
		OwnerASC->ApplyGameplayEffectSpecToSelf(*EncounterEffectSpecHandle.Data.Get());
		TargetASC->ApplyGameplayEffectSpecToSelf(*TargetEffectSpecHandle.Data.Get());

		// Owner에게 CameraShake 적용
		FGameplayCueParameters CueParams;
		FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
		EffectContext.AddInstigator(GetOwner(), GetOwner());
		EffectContext.AddSourceObject(OwnerASC);
    
		CueParams.EffectContext = EffectContext;
		CueParams.NormalizedMagnitude = Message.HitStrengthTag.MatchesTag(SharedTags::Event_Shared_HitReact_Heavy) ? 1.0f : 0.5f;
    
		OwnerASC->ExecuteGameplayCue(GameplayCueTags::GameplayCue_Shared_CameraShake, CueParams);
    
		// Target에게 CameraShake 적용
		FGameplayCueParameters TargetCueParams;
		FGameplayEffectContextHandle TargetEffectContext = TargetASC->MakeEffectContext();
		TargetEffectContext.AddInstigator(Target, Target);
		TargetEffectContext.AddSourceObject(TargetASC);
    
		TargetCueParams.EffectContext = TargetEffectContext;
		TargetCueParams.NormalizedMagnitude = Message.HitStrengthTag.MatchesTag(SharedTags::Event_Shared_HitReact_Heavy) ? 1.0f : 0.5f;
    
		TargetASC->ExecuteGameplayCue(GameplayCueTags::GameplayCue_Shared_CameraShake, TargetCueParams);
	}
}

// Damage Effect Handle 팩토리
// Message : Hit 데이터
// Attacker : 공격자
// Victim : 피격자
FGameplayEffectSpecHandle UCombatComponent::GenerateDamageEffectHandle(const FHitReactMessage& Message, AActor* Attacker,
                                                                  AActor* Victim) const
{
	if (not IsPlayersHasASC(Attacker, Victim))
	{
		return nullptr;
	}
	
	UAbilitySystemComponent* VictimASC = GetActorASCInMap(Victim);
	UAbilitySystemComponent* AttackerASC = GetActorASCInMap(Attacker);
	if (not VictimASC || not AttackerASC)
	{
		UE_LOG(LogRedCServer, Warning, TEXT("OwnerASC or AttackerASC is not valid. %s"), *GetNameSafe(this));
		return nullptr;
	}
	
	// Damage Effect 적용
	FGameplayEffectContextHandle DamageEffectContext = VictimASC->MakeEffectContext();
	DamageEffectContext.AddSourceObject(VictimASC);
	DamageEffectContext.AddInstigator(Attacker, Victim);

	FGameplayEffectSpecHandle SpecHandle = VictimASC->MakeOutgoingSpec(
		UApplyDamageEffect::StaticClass(),
		1.0f,
		DamageEffectContext
	);

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_SkillPower, Message.DamageAmount);
		SpecHandle.Data->SetSetByCallerMagnitude(DataTags::Data_SetByCaller_SkillMultiplier, Message.DamageMultiplier);

		return SpecHandle;
	}
	else
	{
		UE_LOG(LogRedC, Warning, TEXT("Damage Spechandle is not Valid. Might be DamageEffect is not registered or invalid"));
		return nullptr;
	}
}

// Encounter Effect 팩토리
// Target : 적용하고자 하는 대상
FGameplayEffectSpecHandle UCombatComponent::GenerateEncounterEffectHandle(AActor* Target) const
{
	if (UAbilitySystemComponent* TargetASC = GetActorASCInMap(Target))
	{
		// Encounter Effect 적용(전투상태 돌입)
		FGameplayEffectContextHandle EncounterEffectContext = TargetASC->MakeEffectContext();
		EncounterEffectContext.AddSourceObject(Target);

		FGameplayEffectSpecHandle EncounterSpecHandle = TargetASC->MakeOutgoingSpec(
			UEncounterEffect::StaticClass(),
			1.0f,
			EncounterEffectContext
		);

		if (EncounterSpecHandle.IsValid())
		{
			return EncounterSpecHandle;
		}
		else
		{
			UE_LOG(LogRedC, Warning, TEXT("Encounter Spechandle is not Valid. Might be EncounterEffect is not registered or invalid"));
			return nullptr;	
		}
	}
	else
	{
		UE_LOG(LogRedC, Error, TEXT("UCombatComponent::GenerateEncounterEffectHandle  Target ASC invalid"));
		return nullptr;
	}
}

#pragma endregion

#pragma region Validation Check
// 스킬 사정거리에 약간의 가중치를 더한 값을 이용한 서버 검증절차
bool UCombatComponent::IsHitValid(const FHitReactMessage& Message, const AActor* Attacker) const
{
	if (not GetOwner() || not IsValid(Attacker)) return false;

	float ValidationDistance = FVector::Dist(GetOwner()->GetActorLocation(), Attacker->GetActorLocation());
	if (ValidationDistance > Message.MaximumAttackLength)
	{
		return false;
	}
	return true;
}

// 공격자와 피격자의 ASC 유무를 확인하고, 없다면 캐싱까지 진행하는 메소드
// 그 결과를 bool으로 반환함
bool UCombatComponent::IsPlayersHasASC(AActor* Attacker, AActor* Victim) const
{
	if (Attacker == nullptr || Victim == nullptr)
	{
		return false;
	}
	
	if (UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Victim) && UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Attacker))
	{
		if (not GetActorASCInMap(Victim))
		{
			SetActorASCInMap(Victim);
		}
		if (not GetActorASCInMap(Attacker))
		{
			SetActorASCInMap(Attacker);
		}
		return GetActorASCInMap(Victim) && GetActorASCInMap(Attacker);
	}
	return false;
}
#pragma endregion

void UCombatComponent::ExecuteGameplayCue(const FHitReactMessage& Message, AActor* Attacker) const
{
	if (UAbilitySystemComponent* ASC = TryGetActorASCInMap(Attacker))
	{
		if (Message.GameplayCueTag.IsValid())
		{
			FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();

			EffectContextHandle.AddInstigator(Attacker, Attacker);
			EffectContextHandle.AddOrigin(Message.ImpactLocation);
			
			if (FCustomGameplayEffectContext* EffectContext = static_cast<FCustomGameplayEffectContext*>(EffectContextHandle.Get()))
			{
				EffectContext->SetCustomVector(Message.AttackDirection);
			}
			
			ASC->ExecuteGameplayCue(Message.GameplayCueTag, EffectContextHandle);
		}
	}
}

void UCombatComponent::ResetCombatState(AActor* Target)
{
	if (UAbilitySystemComponent* ASC = TryGetActorASCInMap(Target))
	{
		ASC->RemoveActiveEffects(FGameplayEffectQuery());
		
		TArray<FGameplayAbilitySpec> Abilities = ASC->GetActivatableAbilities();
		ASC->CancelAllAbilities();
	}
}
