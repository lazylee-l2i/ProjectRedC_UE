#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseStarterWall.generated.h"

UCLASS()
class PROJECTREDC_API ABaseStarterWall : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseStarterWall();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* WallMesh;
};
