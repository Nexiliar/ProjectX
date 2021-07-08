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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwitchWeapon, FName, WeaponIdName, FAddicionalWeaponInfo, WeaponAdditionalInfo, int32, NewCurrentIndexWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAmmoChange, EWeaponType, TypeAmmo, int32, Count, int32, MaxCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAdditionalInfoChange, int32, IndexSlot, FAddicionalWeaponInfo, Additionalnfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmpty, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponInit, FAddicionalWeaponInfo, WeaponInfo,FName, WeaponName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateWeaponSlot, int32, IndexSlotChange, FWeaponSlot, NewInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmountOfItemsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryWidgetCreate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemUsed, int32, AmountOfItemsToUse, FInventory, InventorySlotInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnEquipItem);



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
		FOnWeaponAdditionalInfoChange OnWeaponAdditionalInfoChange;
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
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		TArray<FWeaponSlot> WeaponSlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		TArray<FAmmoSlot> AmmoSlots;


	
	//Equpment	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EqipmentSlots")
		TArray<FInventory> EquipmentSlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EqipmentSlots")
		FVector VariableToSpawnEquip;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EqipmentSlots")
	//	EEquipmentSlotType EquipmentSlotType;

	


	//TestInventory
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlots")
		FInventory  CurrentInitializedInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlots")
		TArray<FInventory> InventorySlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		TSubclassOf<class APickUpActor> PickUpActor = nullptr;
	

	//максимальное количество слотов оружи€
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
		int32 MaxSlotWeapon = 0;
	
	bool SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAddicionalWeaponInfo OldInfo, bool bIsForward);
	UFUNCTION(BlueprintCallable)
	FAddicionalWeaponInfo GetAdditionalInfoWeapon(int32 IndexWeapon);
	UFUNCTION(BlueprintCallable)
	int32 GetWeaponIndexSlotByName(FName IdWeaponName);
	UFUNCTION(BlueprintCallable)
	FName GetWeaponNameBySlotIndex(int32 IndexSlot);
	void SetAdditionalInfoWeapon(int32 IndexWeapon, FAddicionalWeaponInfo Newinfo);


	
	UFUNCTION(BlueprintCallable)
		void AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CountAmmoChange);
	UFUNCTION(BlueprintCallable)
		bool CheckAmmoForWeapon(EWeaponType TypeWeapon, int32 &AvailableAmmoForWeapon);
	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool CheckCanTakeAmmo(EWeaponType AmmoType);
	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool CheckCanTakeWeapon(int32& FreeSlot);
	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot, int32 CurrentIndexWeaponChar, FDropItem& DropItemInfo);
	UFUNCTION(BlueprintCallable, Category = "Interface")
		void TryGetWeaponToInventory(FWeaponSlot NewWeapon);
	UFUNCTION(BlueprintCallable, Category = "Interface")
		bool GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem &DropItemInfo);
	


	

	//Ёкипировка
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class APickUpActor* ItemForPickUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		bool isBodyKitEquiped = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		bool isBackPackEquiped = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
		bool isArmorEquiped = false;
	UFUNCTION(BlueprintCallable)
		bool EquipItem(int32 SlotIndex);
	UFUNCTION(BlueprintCallable)
		bool UnequipItem(int32 SlotIndex, FInventory& IventorySlotInfo);
	UFUNCTION(BlueprintCallable)
		void EquipBackPack(FInventory BackPackInfo, TArray<FInventory> InventorySlotsInfo, int32 InventorySize, bool & EquipSuccessfuly);
	UFUNCTION(BlueprintCallable)
		void UnequipBackPack(int32 SlotIndex);

	//»нвентарь
	UFUNCTION(BlueprintCallable)
		bool CheckWeaponSlotEmpty(int32 SlotIndex, bool IsWeapon);
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
		void InitInventory(TArray<FWeaponSlot> NewWeaponSlotsInfo, TArray<FAmmoSlot> NewAmmoSlotsInfo);


};
