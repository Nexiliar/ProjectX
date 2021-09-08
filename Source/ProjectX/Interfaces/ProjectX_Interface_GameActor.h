// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectX/Weapons/ProjectX_StateEffect.h"
#include "ProjectX/FunctionLibrary/Types.h"
#include "ProjectX_Interface_GameActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UProjectX_Interface_GameActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTX_API IProjectX_Interface_GameActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Event")
	//	void AvailableForEffectsBP();
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	//	bool AvailableForEffects();

	virtual EPhysicalSurface GetSurfaceType();

	virtual TArray<UProjectX_StateEffect*> GetAllCurrentEffects();
	virtual void RemoveEffect(UProjectX_StateEffect* RemoveEffect);
	virtual void AddEffect(UProjectX_StateEffect* NewEffect);
	virtual void GetSpawnLocationForEffect(FVector&MeshLocation, FName &BoneName);


};
