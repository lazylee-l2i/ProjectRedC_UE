// Fill out your copyright notice in the Description page of Project Settings.



#include "Types/RedCGameplayTags.h"

namespace CharacterTags
{
	UE_DEFINE_GAMEPLAY_TAG(Character_Warrior,								"Character.Warrior");
	UE_DEFINE_GAMEPLAY_TAG(Character_Archer,								"Character.Archer");
	UE_DEFINE_GAMEPLAY_TAG(Character_Minion,								"Character.Minion");
	UE_DEFINE_GAMEPLAY_TAG(Character_Tower,									"Character.Tower");
	UE_DEFINE_GAMEPLAY_TAG(Character_None,									"Character.None");
}

namespace AssetTags
{
	UE_DEFINE_GAMEPLAY_TAG(Asset_Character_CharacterStats,					"Asset.Character.CharacterStats");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Character_CommonAbilitySet,				"Asset.Character.CommonAbilitySet");
	
	UE_DEFINE_GAMEPLAY_TAG(Asset_Character_Definition,						"Asset.Character.Definition");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Character_Definition_Warrior,				"Asset.Character.Definition.Warrior");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Character_Definition_Archer,				"Asset.Character.Definition.Archer");

	// Montage Data Asset
	UE_DEFINE_GAMEPLAY_TAG(Asset_Character_Montage,					"Asset.Character.Montage");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Character_Montage_Warrior,			"Asset.Character.Montage.Warrior");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Character_Montage_Archer,			"Asset.Character.Montage.Archer");
	UE_DEFINE_GAMEPLAY_TAG(Asset_Character_Montage_Minion,			"Asset.Character.Montage.Minion");
}

namespace DataTags
{
	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_AttackPower,					"Data.SetByCaller.AttackPower");
	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_Critical,						"Data.SetByCaller.Critical");
	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_MaxHealth,						"Data.SetByCaller.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_Evasion,						"Data.SetByCaller.Evasion");
	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_Defense,						"Data.SetByCaller.Defense");
	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_MovementSpeed,					"Data.SetByCaller.MovementSpeed");
	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_MaxStamina,						"Data.SetByCaller.MaxStamina");
	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_Health,							"Data.SetByCaller.Health");
	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_SkillPower,						"Data.SetByCaller.SkillPower");
	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_SkillMultiplier,				"Data.SetByCaller.SkillMultiplier");
	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_Coin,							"Data.SetByCaller.Coin");

	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_CoolDown,						"Data.SetByCaller.CoolDown")
	UE_DEFINE_GAMEPLAY_TAG(Data_SetByCaller_RespawnTimer,					"Data.SetByCaller.RespawnTimer");
}

namespace InputTags
{
	// Native Input Action
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Native_Look,						"Input.Action.Native.Look");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Native_Move,						"Input.Action.Native.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Native_LMouse,						"Input.Action.Native.LMouse");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Native_RMouse,						"Input.Action.Native.RMouse");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Native_LShift,						"Input.Action.Native.LShift");
	//UE_DEFINE_GAMEPLAY_TAG(Input_Action_Native_ShopSlot,					"Input.Action.Native.ShopSlot");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Native_ShopSlot_Change,				"Input.Action.Native.ShopSlot.Change");

	// Ability Input Action
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability,							"Input.Action.Ability");
	
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_LAttack,					"Input.Action.Ability.LAttack");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_RAttack,					"Input.Action.Ability.RAttack");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_LRAttack,					"Input.Action.Ability.LRAttack");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_SLAttack,					"Input.Action.Ability.SLAttack");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_SRAttack,					"Input.Action.Ability.SRAttack");
	
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_Special,					"Input.Action.Ability.Special");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_Ultimate,					"Input.Action.Ability.Ultimate");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_Grab,						"Input.Action.Ability.Grab");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_Dodge,						"Input.Action.Ability.Dodge");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_WheelUp,					"Input.Action.Ability.WheelUp");
	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_Sprint,						"Input.Action.Ability.Sprint");

	UE_DEFINE_GAMEPLAY_TAG(Input_Action_Ability_ItemBuy,					"Input.Action.Ability.ItemBuy");;

	// Input Mapping Context
	UE_DEFINE_GAMEPLAY_TAG(Input_Context_Basic,								"Input.Context.Basic");
	UE_DEFINE_GAMEPLAY_TAG(Input_Context_Combat,							"Input.Context.Combat");
}

namespace GameplayCueTags
{
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Warrior_Attack_LAttack,				"GameplayCue.Warrior.Attack.LAttack");

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Archer_Projectile_A,					"GameplayCue.Archer.Projectile.A");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Archer_Projectile_B,					"GameplayCue.Archer.Projectile.B");
	
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Player_Get_Coin,						"GameplayCue.Player.Get.Coin");

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Shared_CameraShake,					"GameplayCue.Shared.CameraShake");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Shared_Sprint,						"GameplayCue.Shared.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Shared_HitReact_Light,				"GameplayCue.Shared.HitReact.Light");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Shared_HitReact_Heavy,				"GameplayCue.Shared.HitReact.Heavy");

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Shared_DamageNumber,					"GameplayCue.Shared.DamageNumber");
}

namespace SharedTags
{
	// Gameplay Event
	UE_DEFINE_GAMEPLAY_TAG(Event_Call_End,									"Event.Call.End");
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_Attack_Damage,						"Event.Shared.Attack.Damage");
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_Attack_Grab,						"Event.Shared.Attack.Grab");
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_HitReact_Light,						"Event.Shared.HitReact.Light");
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_HitReact_Heavy,						"Event.Shared.HitReact.Heavy");
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_HitReact_Grab,						"Event.Shared.HitReact.Grab");

	// Combo Event
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_Change,								"Event.Shared.Change");
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_Change_Combo1,						"Event.Shared.Change.Combo1");
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_Change_Combo2,						"Event.Shared.Change.Combo2");
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_Change_Combo3,						"Event.Shared.Change.Combo3");
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_Change_Combo4,						"Event.Shared.Change.Combo4");
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_Change_End,							"Event.Shared.Change.End");

	// Projectile Event
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_ProjectileSpawn,					"Event.Shared.ProjectileSpawn");

	// Item Event
	UE_DEFINE_GAMEPLAY_TAG(Event_Ability_ItemBuy,							"Event.Ability.ItemBuy");
	UE_DEFINE_GAMEPLAY_TAG(Event_Shared_ItemCoolDown,						"Event.Shared.ItemCoolDown");

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Shared_CameraShake,					"GameplayCue.Shared.CameraShake");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Shared_HitReact_Light,				"GameplayCue.Shared.HitReact.Light");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Shared_HitReact_Heavy,				"GameplayCue.Shared.HitReact.Heavy");
}

namespace PlayerStateTags
{
	UE_DEFINE_GAMEPLAY_TAG(Player_Mode_InCombat,							"Player.Mode.InCombat");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_State_Combat_Airborne,					"Player.State.Combat.Airborne");
	UE_DEFINE_GAMEPLAY_TAG(Player_State_Combat_KnockedDown,					"Player.State.Combat.KnockedDown");

	UE_DEFINE_GAMEPLAY_TAG(Player_State_Lock_Look,							"Player.State.Lock.Look");
	UE_DEFINE_GAMEPLAY_TAG(Player_State_Lock_Move,							"Player.State.Lock.Move");
	UE_DEFINE_GAMEPLAY_TAG(Player_State_Lock_Sprint,						"Player.State.Lock.Sprint");

	UE_DEFINE_GAMEPLAY_TAG(Player_State_Death,								"Player.State.Death");
	UE_DEFINE_GAMEPLAY_TAG(Player_State_Respawn,							"Player.State.Respawn");
	UE_DEFINE_GAMEPLAY_TAG(Player_State_KnockedDownRecover,					"Player.State.KnockedDownRecover");
	UE_DEFINE_GAMEPLAY_TAG(Player_State_Invincible,							"Player.State.Invincible");
}

namespace PlayerAbilityTags
{
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack,							"Player.Ability.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_Grab,						"Player.Ability.Attack.Grab");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_LAttack,					"Player.Ability.Attack.LAttack");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_SLAttack,					"Player.Ability.Attack.SLAttack");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_SRAttack,					"Player.Ability.Attack.SRAttack");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_RAttack,					"Player.Ability.Attack.RAttack")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_LRAttack,					"Player.Ability.Attack.LRAttack")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_QAttack,					"Player.Ability.Attack.QAttack")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_EAttack,					"Player.Ability.Attack.EAttack")
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Attack_SBAttack,					"Player.Ability.Attack.SBAttack")

	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Buff,								"Player.Ability.Buff");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Buff_WheelUp,						"Player.Ability.Buff.WheelUp");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_ItemBuy,							"Player.Ability.ItemBuy");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Movement,							"Player.Ability.Movement");
	UE_DEFINE_GAMEPLAY_TAG(Player_Ability_Movement_Sprint,					"Player.Ability.Movement.Sprint");
}

// Effect Tag
namespace PlayerEffectTags
{
	UE_DEFINE_GAMEPLAY_TAG(Player_Effect_Start,								"Player.Effect.Start");
	UE_DEFINE_GAMEPLAY_TAG(Player_Effect_Respawn,							"Player.Effect.Respawn");

	UE_DEFINE_GAMEPLAY_TAG(Player_Effect_Speed,								"Player.Effect.Speed");
	UE_DEFINE_GAMEPLAY_TAG(Player_Effect_Speed_Forward,						"Player.Effect.Speed.Forward");
	UE_DEFINE_GAMEPLAY_TAG(Player_Effect_Speed_Backward,					"Player.Effect.Speed.Backward");
	UE_DEFINE_GAMEPLAY_TAG(Player_Effect_Speed_Sprint,						"Player.Effect.Speed.Sprint");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Effect_DecreaseCoin,						"Player.Effect.DecreaseCoin");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Effect_Buff_RegenStamina,					"Player.Effect.Buff.RegenStamina");

	UE_DEFINE_GAMEPLAY_TAG(Player_Effect_Debuff_DrainStamina,				"Player.Effect.Debuff.DrainStamina");
}

namespace ProjectileTags
{
	UE_DEFINE_GAMEPLAY_TAG(Projectile_Archer_TypeA,		"Projectile.Archer.TypeA")
}

namespace AiAbilityTags
{

	UE_DEFINE_GAMEPLAY_TAG(AI_Ability_Melee,								"AI.Ability.Melee")
	UE_DEFINE_GAMEPLAY_TAG(AI_Ability_Hit,									"AI.Ability.Hit")
	UE_DEFINE_GAMEPLAY_TAG(AI_Ability_Stunned,								"AI.Ability.Stunned")
	UE_DEFINE_GAMEPLAY_TAG(AI_Ability_Death,								"AI.Ability.Death")
	
}
