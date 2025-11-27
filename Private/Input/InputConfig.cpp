// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/InputConfig.h"
#include "InputMappingContext.h"         
#include "EnhancedActionKeyMapping.h"   
#include "InputAction.h"                


const UInputAction* UInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	if (const UInputAction* InputAction = NativeActionMap.FindChecked(InputTag))
	{
		return InputAction;
	}

	return nullptr;
}

const UInputAction* UInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag) const
{
	if (const UInputAction* InputAction = AbilityActionMap.FindChecked(InputTag))
	{
		return InputAction;
	}

	return nullptr;
}


TPair<UInputMappingContext*, int32> UInputConfig::FindInputMappingContextForTag(const FGameplayTag& ContextTag) const
{
	const FInputMappingContextAndPriority* InputMappingContextAndPriority = InputMappingContextMap.Find(ContextTag);

	if (InputMappingContextAndPriority->InputMappingContext)
	{
		return TPair<UInputMappingContext*, int32>(InputMappingContextAndPriority->InputMappingContext, InputMappingContextAndPriority->Priority);
	}

	return TPair<UInputMappingContext*, int32>();
}
