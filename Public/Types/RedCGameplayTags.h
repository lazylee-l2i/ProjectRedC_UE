// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace CharacterTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Warrior);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Archer);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Minion);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Tower);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_None);
}

namespace AssetTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Character_CharacterStats);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Character_CommonAbilitySet);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Character_Definition);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Character_Definition_Warrior);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Character_Definition_Archer);

	// Montage Data Asset
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Character_Montage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Character_Montage_Warrior);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Character_Montage_Archer);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Asset_Character_Montage_Minion);
}

namespace DataTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_AttackPower);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_Critical);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_MaxHealth);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_Evasion);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_Defense);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_MovementSpeed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_MaxStamina);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_Health);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_SkillPower);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_SkillMultiplier);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_Coin);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_CoolDown);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_SetByCaller_RespawnTimer);
}

namespace InputTags
{
	// Native Input Action
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Native_Look);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Native_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Native_LMouse);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Native_RMouse);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Native_LShift);
	//UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Native_ShopSlot);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Native_ShopSlot_Change);

	// Ability Input Action
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability_LAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability_RAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability_LRAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability_SLAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability_SRAttack);
	// Q 스킬
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability_Special);
	// E 스킬
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability_Ultimate);
	// F 스킬
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability_Grab);
	// 스페이스바
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability_Dodge);
	// 휠 업 스킬
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability_WheelUp);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability_Sprint);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Action_Ability_ItemBuy);

	// Input Mapping Context
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Context_Basic);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Context_Combat);
}

namespace GameplayCueTags
{
	/*
	 *		이 부분이 GameplayCue를 통해 VFX를 출력시킬 때 필요한 Tag인데,
	 *		단순하게, GameplayCue.Hit.Light, Heavy으로 하면 캐릭터마다 필요한 VFX를
	 *		TMap으로 만들든, Finder Utility를 만들든 해야하고, 그렇지않으면
	 *		아래처럼 캐릭터들의 스킬에 대한 Tag를 만들어 써야함.
	 *		고민중인 부분으로 일단, 캐릭터의 스킬마다 GameplayCue를 만드는 방향으로 진행하겠음.
	 *		Keep KISS Principle
	 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Warrior_Attack_LAttack);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Archer_Projectile_A);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Archer_Projectile_B);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Player_Get_Coin);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_CameraShake);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_Sprint);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_HitReact_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_HitReact_Heavy);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_DamageNumber);
}

namespace SharedTags
{
	// Gameplay Event
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Call_End);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_Attack_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_Attack_Grab);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_HitReact_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_HitReact_Heavy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_HitReact_Grab);

	// Combo Event
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_Change);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_Change_Combo1);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_Change_Combo2);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_Change_Combo3);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_Change_Combo4);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_Change_End);

	// Projectile Event
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_ProjectileSpawn);

	// Item Event
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Ability_ItemBuy);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Shared_ItemCoolDown);
	
	// Gameplay Cue
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_CameraShake);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_HitReact_Light);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Shared_HitReact_Heavy);
}

namespace PlayerStateTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Mode_InCombat);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_State_Combat_Airborne);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_State_Combat_KnockedDown);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_State_Lock_Look);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_State_Lock_Move);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_State_Lock_Sprint);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_State_Death);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_State_Respawn);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_State_KnockedDownRecover);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_State_Invincible);
}

namespace PlayerAbilityTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Grab);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_LAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_SLAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_SRAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_RAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_LRAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_QAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_EAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_SBAttack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_ItemBuy);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Buff);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Buff_WheelUp);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Movement);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Movement_Sprint);
}

namespace PlayerEffectTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Effect_Start);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Effect_Respawn);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Effect_Speed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Effect_Speed_Forward);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Effect_Speed_Backward);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Effect_Speed_Sprint);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Effect_DecreaseCoin);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Effect_Buff_RegenStamina);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Effect_Debuff_DrainStamina);
}

namespace ProjectileTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Projectile_Archer_TypeA);
}

namespace AiAbilityTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Ability_Melee);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Ability_Hit);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Ability_Stunned);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Ability_Death);
}
