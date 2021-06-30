// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h "
//#include "ProjectX/Character/ProjectXInventoryComponent.h"
#include "Materials/Material.h"
#include "ProjectX/FunctionLibrary/Types.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectX/Game/ProjectXGameInstance.h"
#include "ProjectX/Character/ProjectXCharacter.h"
#include "PickUpActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponTaken, FWeaponSlot, WeaponInfo);

UCLASS()
class PROJECTX_API APickUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USphereComponent* SphereComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USkeletalMeshComponent* SkeletalMesh = nullptr;
	UPROPERTY(EditAnywhere)
		class UMaterialInstance* CloseToPickUp;
	UPROPERTY(EditAnywhere)
		class UMaterialInstance* GetOutOfWeapon;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	FOnWeaponTaken OnWeaponTaken;
	
	AProjectXCharacter* Character = nullptr;

	//AProjectXCharacter* CharacterP = nullptr;

	UPROPERTY()
	bool isOverlapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemCFG")
		bool isWeapon;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemCFG")
	//	FWeaponSlot DroppedWeaponInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemCFG")
		FDropItem WeaponOnGroundInfo;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemCFG")
		int32 AmountItemsTospawn = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemCFG")
		TSubclassOf<class AMasterItem> MasterItem =nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemCFG")
		TSubclassOf<class APickUpActor> ActorForPickUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemCFG")
		FName NameOfTheItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		FInventorySlot Inventory;


	UFUNCTION()
		void CollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void CollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
		void TryToPickUpItem();

	UFUNCTION()
		void DropWeaponOnSwitch(FDropItem DroppedItemCFG);


};
