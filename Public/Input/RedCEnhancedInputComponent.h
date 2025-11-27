// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "InputConfig.h"
#include "Types/RedCGameplayTags.h"
#include "RedCEnhancedInputComponent.generated.h"

class UInputConfig;

/**
 * 
 */
UCLASS()
class PROJECTREDC_API URedCEnhancedInputComponent : public UEnhancedInputComponent
{
   GENERATED_BODY()
   
public:
   template<class UserClass, typename FuncType>
   void BindNativeAction(const UInputConfig* InputConfig, const FGameplayTag& InputTag,
      ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func);

   template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
   void BindAbilityActions(const UInputConfig* InputConfig,
      UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);
   template <class UserClass, class PressedFuncType>
   void BindOnlyKey(const UInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc);
};

template <class UserClass, typename FuncType>
void URedCEnhancedInputComponent::BindNativeAction(const UInputConfig* InputConfig, const FGameplayTag& InputTag,
   ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
{
   check(InputConfig);

   if (const UInputAction* InputAction = InputConfig->FindNativeInputActionForTag(InputTag))
   {
      BindAction(InputAction, TriggerEvent, Object, Func);
   }
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void URedCEnhancedInputComponent::BindAbilityActions(const UInputConfig* InputConfig,
   UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
   check (InputConfig);

   for (const auto& InputAction : InputConfig->AbilityActionMap)
   {
      if (InputAction.Value /* Input Action */)
      {
         if (PressedFunc)
         {
            BindAction(InputAction.Value /* Input Action */, ETriggerEvent::Started, Object, PressedFunc, InputAction.Key /* Input Tag */);
         }
   
         if (ReleasedFunc)
         {
            BindAction(InputAction.Value /* Input Action */, ETriggerEvent::Completed, Object, ReleasedFunc, InputAction.Key /* Input Tag */);
         }
      }
   }
}



template <class UserClass, typename PressedFuncType>
void URedCEnhancedInputComponent::BindOnlyKey(const UInputConfig* InputConfig,
   UserClass* Object, PressedFuncType PressedFunc)
{
   check (InputConfig);

   for (const auto& Pair : InputConfig->AbilityActionMap) // TMap<FGameplayTag, UInputAction*>
   {
      UInputAction* const Action  = Pair.Value;
      if (!Action) continue;

      if (!Pair.Key.MatchesTag( InputTags::Input_Action_Ability_ItemBuy))
      {
         continue;
      }

      // 핸들러 시그니처: void OnPressed/OnReleased(const FInputActionValue& Value, FGameplayTag Tag)
      if (PressedFunc)
      {
         BindAction(Action, ETriggerEvent::Started, Object, PressedFunc);
      }
   }
}
