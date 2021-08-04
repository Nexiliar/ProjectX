// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/Array.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
//#include "ProjectX/Items/PickUpActor.h"
#include "ProjectX/Items/MasterItem.h"
#include "ProjectX/FunctionLibrary/Types.h"
#include "ProjectXInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FWeaponSlotTest
{
	GENERATED_BODY()


		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSlot")
		TArray<FWeaponSlot> Slots;

}; 

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwitchWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAmmoChange, EWeaponType, TypeAmmo, int32, Count, int32, MaxCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponEventAmmoChange, int32, IndexSlot, int32, Ammo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmpty, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponInit, int32, AmmoInMag,FWeaponInfo, WeaponInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateWeaponSlot, int32, IndexSlotChange, FWeaponSlot, NewInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmountOfItemsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryWidgetCreate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUsed, int32, AmountOfItemsToUse, FInventory, InventorySlotInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnEquipItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeightChange, float, CurrentWeight, float, MaxWeight);




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTX_API UProjectXInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectXInventoryComponent();
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "WeaponEquipment")
		FOnSwitchWeapon OnSwitchWeapon;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnAmmoChange OnAmmoChange;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnWeaponEventAmmoChange OnWeaponEventAmmoChange;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnWeaponAmmoEmpty OnWeaponAmmoEmpty;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnWeaponInit OnWeaponInit;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnUpdateWeaponSlot OnUpdateWeaponSlot;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnAmountOfItemsChanged OnAmountOfItemsChanged;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnInventoryWidgetCreate OnInventoryWidgetCreate;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnItemUsed OnItemUsed;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		FOnEquipItem OnEquipItem;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		FOnUnEquipItem OnUnEquipItem;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		FOnCurrentWeightChange OnCurrentWeightChange;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	



	//Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		TArray<FWeaponInfo> WeaponSlotsInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		TArray<FAmmoSlot> AmmoSlots;
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void SetCurrentAmmo(int32 IndexWeapon, int32 CurrentAmmo);
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		FWeaponInfo GetWeaponInfo(int32 SlotIndex);
	UFUNCTION(BlueprintCallable)
		bool EquipWeapon(FInventory ItemInfo, FWeaponInfo InfoOfTheWeapon);
	UFUNCTION(BlueprintCallable)
		bool UnequipWeapon(int32 SlotIndex);
	
	//Equpment	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EqipmentSlots")
		TArray<FInventory> EquipmentSlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EqipmentSlots")
		FVector VariableToSpawnEquip;

	
	

	//BackPackInventory	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlots")
		FInventory  CurrentInitializedInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlots")
		TArray<FInventory> InventorySlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		TSubclassOf<class APickUpActor> PickUpActor = nullptr;
	

	//Weight
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weight")
		float MaxWeightLimit = 120;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weight")
		float CurrentWeight = 0;
	UFUNCTION(BlueprintCallable, Category = "Weight")
		void ChangeCurrentWeight(FInventory ItemInfo,int32 ItemsInSlot, bool Increase);

	//BodyKit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BodyKit")
		TArray<FAmmoSlot> BodyKitAmmoSlotsInfo;
	UPROPERTY()
		FName AmmoPackName;

	UFUNCTION(BlueprintCallable, Category = "BodyKit")
		TArray<FAmmoSlot> GetAmmoSlotsInfo();
	UFUNCTION(BlueprintCallable, Category = "BodyKit")
		void SetAmmoInBodyKit(EWeaponType WeaponType, int32 AmountOfAmmo, int32& AmmoRemain);
	UFUNCTION(BlueprintCallable, Category = "BodyKit")
		void BodyKitAmmoTypeChange(FAmmoSlot AmmoSlotInfo);
	UFUNCTION(BlueprintCallable)
		bool EquipBodyKit(FInventory BodyKitInfo, TArray<FAmmoSlot> BodyKitAmmoSlots);
	UFUNCTION(BlueprintCallable, Category = "BodyKitInit")
		void BodyKitInit(FWeaponInfo WeaponInfo);
	UFUNCTION(BlueprintCallable)
		bool UnequipBodyKit(int32 SlotIndex);

	//Armor
	UPROPERTY(BlueprintReadOnly)
		float CoefForArmor;

	UFUNCTION(BlueprintCallable, Category = "Armor")
		bool EquipArmor(FInventory ItemInfo, float Coef);
	UFUNCTION(BlueprintCallable, Category = "Armor")
		void UnEquipArmor(int32 SlotIndex);


	//Bracer
	UPROPERTY(BlueprintReadOnly)
		float CoolDown;
	UPROPERTY(BlueprintReadOnly)
		float TimeRemaining;
	UPROPERTY(BlueprintReadOnly)
		ESkillList EquipedBracerSkill;
	UFUNCTION(BlueprintCallable, Category = "Bracer")
		bool EquipBracer(FInventory ItemInfo, float BracerSkillCoolDown, float BracerSkillTimer, ESkillList CurrentSkillInBracer);
	UFUNCTION()
		bool UnequipBracer(int32 SlotIndex);


	
	UFUNCTION(BlueprintCallable)
		bool DropItemCFG(APickUpActor* &ItemCFG);

	UFUNCTION(BlueprintCallable)
		bool CheckAmmoForWeapon(EWeaponType TypeWeapon, int32 &AvailableAmmoForWeapon);
	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool CheckCanTakeWeapon(int32& FreeSlot);

	


	

	//Ёкипировка
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class APickUpActor* ItemForPickUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		bool isBodyKitEquiped = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		bool isBackPackEquiped = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		bool isArmorEquiped = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		bool isBracerEquiped = false;

	UFUNCTION(BlueprintCallable)
		bool UnequipItem(int32 SlotIndex, FInventory& IventorySlotInfo);
	UFUNCTION(BlueprintCallable)
		void EquipBackPack(FInventory BackPackInfo, TArray<FInventory> InventorySlotsInfo, int32 InventorySize, bool & EquipSuccessfuly);
	UFUNCTION(BlueprintCallable)
		void UnequipBackPack(int32 SlotIndex);





	//»нвентарь
	//UFUNCTION(BlueprintCallable)
	//	bool CheckWeaponSlotEmpty(int32 SlotIndex, bool IsWeapon);
	UFUNCTION(BlueprintCallable)
		void GetItemInfoAtIndex(int32 SlotIndex,bool& isSlotEmpty, FInventory &InventoryslotInfo);
	UFUNCTION(BlueprintCallable)
		int32 SearchEmptySlotIndex(bool& bIsSucces);
	UFUNCTION(BlueprintCallable)
		int32 SearchFreeStack(FItemsInfo ItemInfo, bool& Success);
	UFUNCTION(BlueprintCallable)
		bool AddItem(FInventory InventoryInfo, int32 AmountOfItems, int32& RestItems);
	UFUNCTION(BlueprintCallable)
		void InventoryInit(FName Name, FInventory& InitializedInventory);
	UFUNCTION(BlueprintCallable)
		int32 GetAmountOfItemsAtIndex(int32 index);
	UFUNCTION(BlueprintCallable)
		bool UseItemAtIndex(int32 ItemIndex, int32 AmountOfItems);
	UFUNCTION(BlueprintCallable)
		bool SwapItemsWithIndex(int32 FirstItemIndex, int32 SecondItemIndex, bool IsWeapon);
	UFUNCTION(BlueprintCallable)
		bool SplitStackByAmount(int32 StackIndex, int32 AmountOfItemsToSplit);
	UFUNCTION(BlueprintCallable)
		bool AddToIndex(int32 FromIndex, int32 ToIndex);
	UFUNCTION(BlueprintCallable)
		bool SplitStackToIndex(int32 FromIndex, int32 ToIndex, int32 AmountToSplit);
	UFUNCTION(BlueprintCallable)
		void DestroyItems(int32 ItemIndex);

	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void InitInventory(TArray<FWeaponInfo> NewWeaponSlotsInfo, TArray<FAmmoSlot> NewAmmoSlotsInfo, TArray<FInventory> NewInventorySlots, TArray<FInventory> NewEquipmentSlotInfo);
	UFUNCTION(BlueprintCallable, Category = "WeaponINIT")
		void WeaponINIT();


};
