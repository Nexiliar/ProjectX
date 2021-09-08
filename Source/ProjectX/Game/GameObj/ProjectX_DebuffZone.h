// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProjectX/FunctionLibrary/Types.h"
#include "ProjectX/Character/ProjectXCharacter.h"
#include "ProjectX/Interfaces/ProjectX_Interface_GameActor.h"
#include "ProjectX_DebuffZone.generated.h"

UCLASS()
class PROJECTX_API AProjectX_DebuffZone : public AActor, public IProjectX_Interface_GameActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectX_DebuffZone();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UBoxComponent* BoxComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UParticleSystemComponent* ZoneFX = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectSettings")
	//	TArray<TSubclassOf<UProjectX_StateEffect>> Effect;
	UPROPERTY(BlueprintReadWrite, Category = "EffectSettings")
		TSubclassOf<UProjectX_StateEffect> Effect = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectSettings")
		EDebuffEffect DeBuffEffect;


	UFUNCTION()
		virtual	void  CollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual	void  CollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
