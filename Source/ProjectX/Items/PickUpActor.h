// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h "
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Materials/Material.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "ProjectX/FunctionLibrary/Types.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectX/Game/ProjectXGameInstance.h"
#include "ProjectX/Character/ProjectXCharacter.h"
#include "PickUpActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponTaken, FWeaponSlot, WeaponInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemZoneBeginOverlap, bool, IsOverlaping);

UCLASS(Blueprintable, BlueprintType)
class PROJECTX_API APickUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpActor();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UBoxComponent* BoxComponent = nullptr;
	UPROPERTY(EditAnywhere)
		class UMaterialInstance* CloseToPickUp;
	UPROPERTY(EditAnywhere)
		class UMaterialInstance* GetOutOfWeapon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		int32 BackPackSlotAmount;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlots")
		TArray<FInventory> InventorySlots;
	
	FOnItemZoneBeginOverlap OnItemZoneBeginOverlap;
	FOnWeaponTaken OnWeaponTaken;
	
	AProjectXCharacter* Character = nullptr;


	UPROPERTY()
	bool isOverlapping =false;
		
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemCFG")
		int32 AmountItemsTospawn = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "ItemCFG")
		FName NameOfTheItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemCFG")
		FWeaponInfo WeaponInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemCFG")
		FInventory Inventory;

	//ItemInit

	UFUNCTION()
		virtual	void  CollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual	void  CollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
		virtual	void  StaticMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
		virtual void TryToPickUpItem();

	UFUNCTION(BlueprintNativeEvent)
		void OverlapStart_BP(bool isOverlaping);

	//BackPack
	UFUNCTION()
		void InitBackPack(FInventory CurrentItemInfo);
	UFUNCTION()
		int32 GetBackPackSlotsAmount();
	UFUNCTION()
		void SetBackPackSlotsAmount(int32 AmountOfSlots);
	UFUNCTION()
		bool EquipBackPack();




	//UFUNCTION()
		//void DropWeaponOnSwitch(FDropItem DroppedItemCFG);


};


/* alt logic
UCLASS()
class PROJECTX_API ABackPackActor : public APickUpActor
{
	GENERATED_BODY()

public:

	
	
protected:
	
		int32 BackPackSlotAmount = 0;

public:

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlots")
		TArray<FInventory> InventorySlots;

	UFUNCTION()
		int32 GetBackPackSlotsAmount();
	UFUNCTION()
		void SetBackPackSlotsAmount(int32 AmountOfSlots);
	 UFUNCTION()
		void InitObject(FInventory CurrentItemInfo) override;
	UFUNCTION()
		bool EquipBackPack();
	


};*/
