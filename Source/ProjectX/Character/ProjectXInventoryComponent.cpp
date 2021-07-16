// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectX/Character/ProjectXInventoryComponent.h"
#include "ProjectX/Items/PickUpActor.h"
#include "ProjectX/Game/ProjectXGameInstance.h"

// Sets default values for this component's properties
UProjectXInventoryComponent::UProjectXInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}


// Called when the game starts
void UProjectXInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	WeaponINIT();

}


// Called every frame
void UProjectXInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//??? is it working? del comm if its
void UProjectXInventoryComponent::SetCurrentAmmo(int32 IndexWeapon, int32 CurrentAmmo)
{
	if (WeaponSlotsInfo.IsValidIndex(IndexWeapon))
	{
		WeaponSlotsInfo[IndexWeapon].CurrentRound = CurrentAmmo;
		OnWeaponEventAmmoChange.Broadcast(IndexWeapon, CurrentAmmo);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ProjectXInventoryComponent::SetAdditionalInfoWeapon = Not correct index weapon - % d"), IndexWeapon);
}

FWeaponInfo UProjectXInventoryComponent::GetWeaponInfo(int32 SlotIndex)
{
	FWeaponInfo WeaponInfoAtIndex = WeaponSlotsInfo[SlotIndex];

	return WeaponInfoAtIndex;
}

bool UProjectXInventoryComponent::EquipWeapon(FInventory ItemInfo, FWeaponInfo InfoOfTheWeapon)
{
	int32 FreeSlotIndex;
	bool isEquipWeapon = false;
	if (CheckCanTakeWeapon(FreeSlotIndex))
	{
		WeaponSlotsInfo[FreeSlotIndex] = InfoOfTheWeapon;
		isEquipWeapon = true;
		ChangeCurrentWeight(ItemInfo, 1, true);
		BodyKitInit(InfoOfTheWeapon);
		//UE_LOG(LogTemp, Warning, TEXT(" UProjectXInventoryComponent::EquipWeapon SUCCES"));

	}
	else
	{
		AProjectXCharacter* Character = Cast<AProjectXCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		int32 CurrentSlot;
		Character->GetCurrentSlot(CurrentSlot);
		if (UnequipWeapon(CurrentSlot))
		{
			ChangeCurrentWeight(ItemInfo, 1, true);
		}
		EquipWeapon(ItemInfo, InfoOfTheWeapon);

		Character->InitWeapon(InfoOfTheWeapon);
		isEquipWeapon = true;

	}

	OnSwitchWeapon.Broadcast();
	return isEquipWeapon;
}

bool UProjectXInventoryComponent::UnequipWeapon(int32 SlotIndex)
{
	bool isSucces = false;
	FVector LocationToDropBackPack = VariableToSpawnEquip;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	FRotator SpawnRotation(1, 1, 1);
	APickUpActor* DropedWeapon = Cast<APickUpActor>(GetWorld()->SpawnActor(PickUpActor, &LocationToDropBackPack, &SpawnRotation, SpawnParams));
	if (DropedWeapon)
	{
		//TempInv - del after reworkpickupweapon
		FInventory TempInv;
		DropedWeapon->ItemInit(WeaponSlotsInfo[SlotIndex].WeaponName, false, InventorySlots, TempInv);
	}
	UProjectXGameInstance* MyGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());
	FInventory ItemInfo;
	MyGI->GetItemInfoByName(WeaponSlotsInfo[SlotIndex].WeaponName, ItemInfo);
	ChangeCurrentWeight(ItemInfo, 1, false);
	BodyKitInit(WeaponSlotsInfo[SlotIndex]);
	WeaponSlotsInfo[SlotIndex] = {};

	return isSucces;
}

void UProjectXInventoryComponent::ChangeCurrentWeight(FInventory ItemInfo, int32 ItemsInSlot, bool Increase)
{
	if (Increase)
	{
		if(CurrentWeight < MaxWeightLimit)		
		CurrentWeight += ItemInfo.ItemsInfo.ItemWeight *ItemsInSlot;		
	}
	else
	{
		//if (CurrentWeight >= 0)
		CurrentWeight -= ItemInfo.ItemsInfo.ItemWeight* ItemsInSlot;
	}
	OnCurrentWeightChange.Broadcast(CurrentWeight);
}

TArray<FAmmoSlot> UProjectXInventoryComponent::GetAmmoSlotsInfo()
{
	return BodyKitAmmoSlotsInfo;
}

void UProjectXInventoryComponent::SetAmmoInBodyKit(EWeaponType WeaponType, int32 AmountOfAmmo, int32& AmmoRemain)
{
	if (isBodyKitEquiped)
	{
		for (int8 i = 0; i < AmmoSlots.Num(); i++)
		{
			if (AmmoSlots[i].WeaponType == WeaponType)
			{
				if (AmmoSlots[i].Count + AmountOfAmmo >= AmmoSlots[i].MaxCount)
				{
					int32 Rem = AmmoSlots[i].MaxCount - AmmoSlots[i].Count;
					AmmoRemain = AmountOfAmmo - Rem;
					AmmoSlots[i].Count = AmmoSlots[i].MaxCount;
				}
				else
				{
					AmmoSlots[i].Count += AmountOfAmmo;
				}
					//Add currentAmmo to HiddenArray of whole ammoslots
					for (int8 j = 0; j < BodyKitAmmoSlotsInfo.Num(); j++)
					{
						if (BodyKitAmmoSlotsInfo[j].WeaponType == AmmoSlots[i].WeaponType)
						{
							BodyKitAmmoSlotsInfo[j] = AmmoSlots[i];
						}						
					}
				//bIsFind = true;
				OnAmmoChange.Broadcast(AmmoSlots[i].WeaponType, AmmoSlots[i].Count, AmmoSlots[i].MaxCount);
			}
			
		}		
	}
}

void UProjectXInventoryComponent::BodyKitAmmoTypeChange(FAmmoSlot AmmoSlotInfo)
{
	EWeaponType CurrentWeaponType = AmmoSlotInfo.WeaponType;
	
	switch (CurrentWeaponType)
	{
	case EWeaponType::Pistol:
		AmmoPackName = "AmmoPistol";
		break;
	case EWeaponType::ShotGunType:
		AmmoPackName = "AmmoShotGun";
		break;
	case EWeaponType::Rifle:
		AmmoPackName = "AmmoRifle";
		break;
	case EWeaponType::GrenadeLauncher:
		AmmoPackName = "AmmoGrenadeLauncher";
		break;
	case EWeaponType::SniperRIfle:
		AmmoPackName = "AmmoSniperRIfle";
		break;
	default:
		break;
	}

	if (isBackPackEquiped )
	{
		if (AmmoSlotInfo.Count != 0)
		{
			UProjectXGameInstance* MyGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());
			FInventory ItemInfo;
			MyGI->GetItemInfoByName(AmmoPackName, ItemInfo);
			int32 RES;
			AddItem(ItemInfo, AmmoSlotInfo.Count, RES);
			
			for (int8 i = 0; i < BodyKitAmmoSlotsInfo.Num(); i++)
			{
				if (BodyKitAmmoSlotsInfo[i].WeaponType == CurrentWeaponType)
					BodyKitAmmoSlotsInfo[i].Count = 0;
			}
		}

	}
	else
	{
		if (AmmoSlotInfo.Count != 0)
		{
			FVector LocationToDropBackPack = VariableToSpawnEquip;
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			SpawnParams.Owner = GetOwner();
			FRotator SpawnRotation(1, 1, 1);
			APickUpActor* DropedAmmo = Cast<APickUpActor>(GetWorld()->SpawnActor(PickUpActor, &LocationToDropBackPack, &SpawnRotation, SpawnParams));
			DropedAmmo->ItemInit(AmmoPackName, true, InventorySlots, CurrentInitializedInventory);
			DropedAmmo->AmountItemsTospawn = AmmoSlotInfo.Count;
			for (int8 i = 0; i < BodyKitAmmoSlotsInfo.Num(); i++)
			{
				if (BodyKitAmmoSlotsInfo[i].WeaponType == CurrentWeaponType)
					BodyKitAmmoSlotsInfo[i].Count = 0;
			}
		}

	}
	
}




/* /DelIF NOT USE
void UProjectXInventoryComponent::AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CountAmmoChange)
{
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			AmmoSlots[i].Count += CountAmmoChange;
			if (AmmoSlots[i].Count > AmmoSlots[i].MaxCount)
				AmmoSlots[i].Count = AmmoSlots[i].MaxCount;
			
			OnAmmoChange.Broadcast(AmmoSlots[i].WeaponType, AmmoSlots[i].Count,AmmoSlots[i].MaxCount);

			bIsFind = true;
			
		}
		i++;
	}
}*/


//Check how much ammo is left 
bool UProjectXInventoryComponent::CheckAmmoForWeapon(EWeaponType TypeWeapon, int32 &AviableAmmoForWeapon)
{
	AviableAmmoForWeapon = 0;
	bool bIsFind = false;
	bool IsEnoughAmmo = false;
	int8 i = 0;
	while (i< AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon )
		{
			bIsFind = true;
			AviableAmmoForWeapon = AmmoSlots[i].Count;
			if (AmmoSlots[i].Count > 0)
				IsEnoughAmmo = true;
		}			
		i++;
	}
	OnWeaponAmmoEmpty.Broadcast(TypeWeapon);
	return IsEnoughAmmo;
}

/* Check if need
bool UProjectXInventoryComponent::CheckCanTakeAmmo(EWeaponType AmmoType)
{
	bool result = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !result)
	{
		if (AmmoSlots[i].WeaponType == AmmoType && AmmoSlots[i].Count < AmmoSlots[i].MaxCount)
			result = true;
		i++;
	}
	return result;
}
*/
//NeedRework delet comm after
bool UProjectXInventoryComponent::CheckCanTakeWeapon(int32 &FreeSlot)
{
	bool bIsFreeslot = false;
	int8 i = 0;
	while (i < WeaponSlotsInfo.Num() && !bIsFreeslot)
	{
		if (WeaponSlotsInfo[i].WeaponClass)
		{
		}
		else
		{	
			if (WeaponSlotsInfo[i].CanUseSlot)
			{
				bIsFreeslot = true;
				FreeSlot = i;
			}
			
		}
		i++;
	}
	return bIsFreeslot;
}


bool UProjectXInventoryComponent::UnequipItem(int32 SlotIndex, FInventory& IventorySlotInfo)
{
	bool bIsFind = false;
	EEquipmentSlotType Equipment = EquipmentSlots[SlotIndex].EquipmentInfo.SlotType;
	
	switch (Equipment)
	{
	case EEquipmentSlotType::None:
		break;
	case EEquipmentSlotType::Weapon:
		UnequipWeapon(SlotIndex);
		WeaponSlotsInfo[SlotIndex] = {};
		UE_LOG(LogTemp, Warning, TEXT(" UnEquipWeapon"));
		bIsFind = true;
		OnUnEquipItem.Broadcast();
		break;
	case EEquipmentSlotType::Bracer:
		break;
	case EEquipmentSlotType::BodyKit:
		UnequipBodyKit(SlotIndex);
		break;
	case EEquipmentSlotType::Armor:
		UnEquipArmor(SlotIndex);
		break;
	case EEquipmentSlotType::BackPack:
		UnequipBackPack(SlotIndex);
		isBackPackEquiped = false;
		InventorySlots.Empty();
		EquipmentSlots[SlotIndex] = {};
		EquipmentSlots[SlotIndex].EquipmentInfo.SlotType = EEquipmentSlotType::BackPack;
		UE_LOG(LogTemp, Warning, TEXT(" UnEquipSucces"));
		bIsFind = true;
		OnUnEquipItem.Broadcast();
		break;
	default:
		break;
	}


	return bIsFind;
}




void UProjectXInventoryComponent::EquipBackPack(FInventory BackPackInfo, TArray<FInventory> InventorySlotsInfo, int32 InventorySize, bool& EquipSuccessfuly)
{		
	for (int8 i = 0; i < EquipmentSlots.Num(); i++)
	{
		if (EquipmentSlots[i].EquipmentInfo.SlotType == EEquipmentSlotType::BackPack)
		{
			if (isBackPackEquiped)
			{
				FInventory TempInv;
				bool EquipSucces = false;
				if (UnequipItem(i, TempInv))
				{
					EquipBackPack(BackPackInfo, InventorySlotsInfo, InventorySize, EquipSucces);
					EquipSuccessfuly = true;
					isBackPackEquiped = true;
				}
			}
			else
			{
				EquipmentSlots[i] = BackPackInfo;
				InventorySlots.SetNum(InventorySize);
				InventorySlots = InventorySlotsInfo;
				EquipSuccessfuly = true;
				isBackPackEquiped = true;
			}
				
		}		
	}
	ChangeCurrentWeight(BackPackInfo, 1, true);
	OnEquipItem.Broadcast();	
}


void UProjectXInventoryComponent::UnequipBackPack(int32 SlotIndex)
{
	
	FVector LocationToDropBackPack = VariableToSpawnEquip;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	FRotator SpawnRotation(1,1,1);
	APickUpActor* DropedBackPack = Cast<APickUpActor>(GetWorld()->SpawnActor(PickUpActor, &LocationToDropBackPack, &SpawnRotation, SpawnParams));
	if (DropedBackPack)
	{
		DropedBackPack->SetBackPackSlotsAmount(InventorySlots.Num());
		DropedBackPack->ItemInit(EquipmentSlots[SlotIndex].ItemsInfo.ItemName, false, InventorySlots, EquipmentSlots[SlotIndex]);

		UProjectXGameInstance* MyGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());
		FInventory ItemInfo;
		MyGI->GetItemInfoByName(EquipmentSlots[SlotIndex].ItemsInfo.ItemName, ItemInfo);
		ChangeCurrentWeight(ItemInfo, 1, false);
	}
	
}

void UProjectXInventoryComponent::GetItemInfoAtIndex(int32 SlotIndex, bool& isSlotEmpty, FInventory& InventoryslotInfo)
{
	int32 IncomeIndex = SlotIndex;
	
		if (InventorySlots[IncomeIndex].ItemsInfo.ItemName == "None")
		{
			isSlotEmpty = true;
		}
		else
		{
			isSlotEmpty = false;
			InventoryslotInfo = InventorySlots[IncomeIndex];
		}

}

int32 UProjectXInventoryComponent::SearchEmptySlotIndex(bool &bIsSucces)
{
	bool bIsFind = false;
	int8 i = 0;
	int8 FoundEmptySlot = 0;
	while (i < InventorySlots.Num() && !bIsFind)
	{		
		//if (InventorySlots[i].ItemsInfo.ItemName == "None")
		if (InventorySlots[i].ItemsInfo.isSlotOccupied)
		{

		}
		else
		{
			FoundEmptySlot = i;
			bIsFind = true;
			bIsSucces = bIsFind;

			//UE_LOG(LogTemp, Warning, TEXT(" UProjectXInventoryComponent::SearchEmptySlotIndex"));
		}
		i++;
	}
	return FoundEmptySlot;
	
}

bool UProjectXInventoryComponent::UnequipBodyKit(int32 SlotIndex)
{
	bool UnequipSuccess = false;
	FVector LocationToDropBackPack = VariableToSpawnEquip;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	FRotator SpawnRotation(1, 1, 1);
	APickUpActor* DropedBodyKit = Cast<APickUpActor>(GetWorld()->SpawnActor(PickUpActor, &LocationToDropBackPack, &SpawnRotation, SpawnParams));
	if (DropedBodyKit)
	{
		for (int8 i = 0; i < AmmoSlots.Num(); i++)
		{
			if (AmmoSlots[i].Count > 0)
			{
				BodyKitAmmoTypeChange(AmmoSlots[i]);
				AmmoSlots[i].Count = 0;
			}

		}
		DropedBodyKit->SetBodyKitInfo(BodyKitAmmoSlotsInfo);
		DropedBodyKit->ItemInit(EquipmentSlots[SlotIndex].ItemsInfo.ItemName, false, InventorySlots, EquipmentSlots[SlotIndex]);		
		
		UnequipSuccess = true;
		isBodyKitEquiped = false;
	}

	ChangeCurrentWeight(EquipmentSlots[SlotIndex], 1, false);
	EquipmentSlots[SlotIndex] = {};
	EquipmentSlots[SlotIndex].EquipmentInfo.SlotType = EEquipmentSlotType::BodyKit;
	//mb not need
	AmmoSlots = {};
	BodyKitAmmoSlotsInfo = {};
	OnEquipItem.Broadcast();
	return UnequipSuccess;
}

bool UProjectXInventoryComponent::EquipArmor(FInventory ItemInfo, float Coef)
{	
	bool isEquipArmorSucces = false;
	
		
		for (int8 i = 0; i < EquipmentSlots.Num(); i++)
		{
			if (EquipmentSlots[i].EquipmentInfo.SlotType == EEquipmentSlotType::Armor)
			{				
				EquipmentSlots[i] = ItemInfo;
				UProjectXHealthComponent* myHealth = Cast<UProjectXHealthComponent>(GetOwner()->GetComponentByClass(UProjectXHealthComponent::StaticClass()));
				if (myHealth)
				{
					if (isArmorEquiped)
					{
						UnEquipArmor(i);
						CoefForArmor = 0.0f;
						EquipArmor(ItemInfo,Coef);
					}
					else
					{
						CoefForArmor = Coef;
						myHealth->CoefDamage = Coef;
						ChangeCurrentWeight(ItemInfo, 1, true);
						OnEquipItem.Broadcast();
						isEquipArmorSucces = true;
						isArmorEquiped = true;
					}				
				}
			}
		}
	
	OnEquipItem.Broadcast();
	return isEquipArmorSucces;
}

void UProjectXInventoryComponent::UnEquipArmor(int32 SlotIndex)
{
	UProjectXHealthComponent* myHealth = Cast<UProjectXHealthComponent>(GetOwner()->GetComponentByClass(UProjectXHealthComponent::StaticClass()));
	if (myHealth)
	{
		myHealth->CoefDamage = 1.0f;
		isArmorEquiped = false;
	}	
	FVector LocationToDropBackPack = VariableToSpawnEquip;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	FRotator SpawnRotation(1, 1, 1);
	APickUpActor* SpawnedItemCFG = Cast<APickUpActor>(GetWorld()->SpawnActor(PickUpActor, &LocationToDropBackPack, &SpawnRotation, SpawnParams));

	if (SpawnedItemCFG)
	{
		SpawnedItemCFG->DefCoef = CoefForArmor;
		SpawnedItemCFG->ItemInit(EquipmentSlots[SlotIndex].ItemsInfo.ItemName, false, InventorySlots, EquipmentSlots[SlotIndex]);
		UE_LOG(LogTemp, Warning, TEXT(" UnEquipArmor SPAWNEDITEMCFG"));
	}
	EquipmentSlots[SlotIndex] = {};
	EquipmentSlots[SlotIndex].EquipmentInfo.SlotType = EEquipmentSlotType::Armor;
	OnEquipItem.Broadcast();
}

bool UProjectXInventoryComponent::DropItemCFG(APickUpActor*& ItemCFG)
{
	bool DropedItemSpawned = false;
	FVector LocationToDropBackPack = VariableToSpawnEquip;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	FRotator SpawnRotation(1, 1, 1);
	APickUpActor* DropedItem = Cast<APickUpActor>(GetWorld()->SpawnActor(PickUpActor, &LocationToDropBackPack, &SpawnRotation, SpawnParams));
	if (DropedItem)
	{
		DropedItemSpawned = true;
	}
	return DropedItemSpawned;
}

bool UProjectXInventoryComponent::EquipBodyKit(FInventory BodyKitInfo, TArray<FAmmoSlot> BodyKitAmmoSlots)
{
	bool EquipSucces = false;
	
	for (int8 i = 0; i < EquipmentSlots.Num(); i++)
	{
		if (EquipmentSlots[i].EquipmentInfo.SlotType == EEquipmentSlotType::BodyKit)
		{
			if (isBodyKitEquiped)
			{
				UnequipBodyKit(i);				
				EquipBodyKit(BodyKitInfo, BodyKitAmmoSlots);
			}
			else
			{
				BodyKitAmmoSlotsInfo = BodyKitAmmoSlots;
				EquipSucces = true;
				FWeaponInfo WeaponSlot;
				BodyKitInit(WeaponSlot);
				isBodyKitEquiped = true;
				EquipmentSlots[i] = BodyKitInfo;
				ChangeCurrentWeight(BodyKitInfo, 1, true);
			}
		}
	}
	OnEquipItem.Broadcast();
	return true;
}


int32 UProjectXInventoryComponent::SearchFreeStack(FItemsInfo ItemInfo,bool &Success)
{
	Success = false;
	int8 i = 0;
	int32 FreeStackSlotIndex = 0;
	while (i < InventorySlots.Num() && !Success)
	{
			if (InventorySlots[i].ItemsInfo.ItemName == ItemInfo.ItemName && InventorySlots[i].ItemsInfo.AmountOfItemsInSlot < ItemInfo.MaxItemsStackAtSlot)
			{
				Success = true;
				FreeStackSlotIndex = i;
				UE_LOG(LogTemp, Warning, TEXT(" UProjectXInventoryComponent::SearchFreeStack: STACK FOUND"));
			}
		i++;
	}
	return FreeStackSlotIndex;
}

bool UProjectXInventoryComponent::AddItem(FInventory InventoryInfo, int32 AmountOfItems, int32 &RestItems)
{
	bool bIsSuccess = false;		
	if (InventoryInfo.ItemsInfo.ItemcanBeStacked)
	{		
		bool FreeStackFound;
		int32 ItemIndex = SearchFreeStack(InventoryInfo.ItemsInfo, FreeStackFound);
		if (FreeStackFound)
		{			
			if (InventorySlots[ItemIndex].ItemsInfo.AmountOfItemsInSlot + AmountOfItems > InventorySlots[ItemIndex].ItemsInfo.MaxItemsStackAtSlot)
			{				
				int32 ResAmountOfItems = InventorySlots[ItemIndex].ItemsInfo.AmountOfItemsInSlot + AmountOfItems - InventorySlots[ItemIndex].ItemsInfo.MaxItemsStackAtSlot;				
				InventorySlots[ItemIndex].ItemsInfo.AmountOfItemsInSlot = InventorySlots[ItemIndex].ItemsInfo.MaxItemsStackAtSlot;
				InventorySlots[ItemIndex].ItemsInfo.isSlotOccupied = true;								
				
				AddItem(InventoryInfo, ResAmountOfItems, RestItems);
				bIsSuccess = true;
				RestItems = RestItems;
				ChangeCurrentWeight(InventoryInfo, AmountOfItems,true);
			}
			else
			{
			
				
				InventorySlots[ItemIndex].ItemsInfo.AmountOfItemsInSlot = InventorySlots[ItemIndex].ItemsInfo.AmountOfItemsInSlot + AmountOfItems;
				InventorySlots[ItemIndex].ItemsInfo.isSlotOccupied = true;
				InventorySlots[ItemIndex].EquipmentInfo = InventoryInfo.EquipmentInfo;
				//InventorySlots[ItemIndex].ItemsInfo = LocalInventoryInit.ItemsInfo;
				bIsSuccess = true;
				RestItems = 0;
				ChangeCurrentWeight(InventoryInfo, AmountOfItems, true);
			}
		}
		else
		{
			
			bool IndexFound;
			int32 EmptySlotIndex=SearchEmptySlotIndex(IndexFound);
			if (IndexFound)
			{

				if (AmountOfItems > InventoryInfo.ItemsInfo.MaxItemsStackAtSlot)
				{
					//InventorySlots[EmptySlotIndex].MasterItem = MasterItem;
					
					InventorySlots[EmptySlotIndex].ItemsInfo = InventoryInfo.ItemsInfo;
					InventorySlots[EmptySlotIndex].ItemsInfo.AmountOfItemsInSlot = InventoryInfo.ItemsInfo.MaxItemsStackAtSlot;
					int32 AmountOfItemsForNewStack = AmountOfItems - InventoryInfo.ItemsInfo.MaxItemsStackAtSlot;
					InventorySlots[EmptySlotIndex].ItemsInfo.isSlotOccupied = true;
					InventorySlots[EmptySlotIndex].EquipmentInfo = InventoryInfo.EquipmentInfo;
					AddItem(InventoryInfo,AmountOfItemsForNewStack,RestItems);
					bIsSuccess = true;
					RestItems = RestItems;
					ChangeCurrentWeight(InventoryInfo, AmountOfItems, true);
				}
				else
				{
					//InventorySlots[EmptySlotIndex].MasterItem = MasterItem;
					
					InventorySlots[EmptySlotIndex].ItemsInfo = InventoryInfo.ItemsInfo;
					InventorySlots[EmptySlotIndex].ItemsInfo.AmountOfItemsInSlot = AmountOfItems;
					InventorySlots[EmptySlotIndex].ItemsInfo.isSlotOccupied = true;
					InventorySlots[EmptySlotIndex].EquipmentInfo = InventoryInfo.EquipmentInfo;
					bIsSuccess = true;
					RestItems = 0;
					ChangeCurrentWeight(InventoryInfo, AmountOfItems, true);
				}
			}
			else
			{
				RestItems = AmountOfItems;
				bIsSuccess = false;
			}
		}
	}
	else
	{
		bool bIsFound = false;
		int32 FoundSlotIndex = SearchEmptySlotIndex(bIsFound);
		if (bIsFound)
		{	
			int32 CurrentAmount = AmountOfItems;
			//InventorySlots[FoundSlotIndex].MasterItem = MasterItem;
			
			InventorySlots[FoundSlotIndex].ItemsInfo = InventoryInfo.ItemsInfo;
			InventorySlots[FoundSlotIndex].ItemsInfo.AmountOfItemsInSlot = 1;
			InventorySlots[FoundSlotIndex].ItemsInfo.isSlotOccupied = true;
			InventorySlots[FoundSlotIndex].EquipmentInfo = InventoryInfo.EquipmentInfo;
			ChangeCurrentWeight(InventoryInfo, 1, true);
			if(CurrentAmount >1)
			{			
				//for (int8 LocalAmountOfItems = AmountOfItems; LocalAmountOfItems > 1; LocalAmountOfItems--)
				for(CurrentAmount; CurrentAmount>1; CurrentAmount--)
				{
					AddItem(InventoryInfo, 1, RestItems);
					//ChangeCurrentWeight(InventoryInfo, 1, true);
				}
				
				RestItems = RestItems;
			}
			else
			{
				RestItems = 0;
			}
			
		bIsSuccess = true;
		}
		else
		{
			bIsSuccess = false;
			RestItems = AmountOfItems;
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("ADD SUCCES"));
	
	OnAmountOfItemsChanged.Broadcast();
	return bIsSuccess;
}



void UProjectXInventoryComponent::InventoryInit(FName Name, FInventory& InitializedInventory)
{

	UProjectXGameInstance* MyGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());
	FInventory myInventory;

	if (MyGI)
	{

		if (MyGI->GetItemInfoByName(Name, myInventory))
		{

			if (myInventory.ItemsInfo.ItemName == "None")
			{		
				//UE_LOG(LogTemp, Warning, TEXT("UProjectXInventoryComponent::InventoryInit ItemName == None"));
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("UProjectXInventoryComponent::InventoryInit ItemName isValid"));
				InitializedInventory = myInventory;
			}
		}
	}


	
}
int32 UProjectXInventoryComponent::GetAmountOfItemsAtIndex(int32 index)
{
	int32 AmountItems = InventorySlots[index].ItemsInfo.AmountOfItemsInSlot;
	

	return AmountItems;
}

bool UProjectXInventoryComponent::UseItemAtIndex(int32 ItemIndex, int32 AmountOfItems)
{
	bool bIsSucces = false;
	bool IsSlotEmpty;
	FInventory LocInv;
	GetItemInfoAtIndex(ItemIndex, IsSlotEmpty, LocInv);
	if (!IsSlotEmpty && AmountOfItems > 0)
	{		
			if (AmountOfItems >= GetAmountOfItemsAtIndex(ItemIndex))
			{
				InventorySlots[ItemIndex].ItemsInfo = {};
				InventorySlots[ItemIndex].ItemsInfo.isSlotOccupied = false;
				bIsSucces = true;
			}
			else
			{
				InventorySlots[ItemIndex].ItemsInfo.AmountOfItemsInSlot -= AmountOfItems;
				bIsSucces = true;
			}
		OnAmountOfItemsChanged.Broadcast();
		OnItemUsed.Broadcast(AmountOfItems,InventorySlots[ItemIndex]);	
	}	
	return bIsSucces;
}

bool UProjectXInventoryComponent::SwapItemsWithIndex(int32 FirstItemIndex, int32 SecondItemIndex, bool IsWeapon)
{
	bool bIsSucces = false;	
	if (IsWeapon)
	{
		FWeaponInfo TempWeaponStorage = WeaponSlotsInfo[SecondItemIndex];
		WeaponSlotsInfo[SecondItemIndex] = WeaponSlotsInfo[FirstItemIndex];
		WeaponSlotsInfo[FirstItemIndex] = TempWeaponStorage;
		bIsSucces = true;
		OnSwitchWeapon.Broadcast();			
	}
	else
	{
		int32 LastInventoryIndex = InventorySlots.Num();

			if (FirstItemIndex > LastInventoryIndex || SecondItemIndex > LastInventoryIndex)
			{

			}
			else
			{			
				UE_LOG(LogTemp, Warning, TEXT(" UProjectXInventoryComponent::SwapItemsWithIndex OurIndexs are match arraysize"));
				FInventory TempElemStorage = InventorySlots[SecondItemIndex];;
				InventorySlots[SecondItemIndex] = InventorySlots[FirstItemIndex];
				InventorySlots[FirstItemIndex] = TempElemStorage;
				bIsSucces = true;	
			}
	}

	OnAmountOfItemsChanged.Broadcast();
	return bIsSucces;
}

bool UProjectXInventoryComponent::SplitStackByAmount(int32 StackIndex, int32 AmountOfItemsToSplit)
{
	bool bIsSucces = false;
	/* 	
	bool IsSlotEmpty;
	FInventory LocInv;
	GetItemInfoAtIndex(StackIndex, IsSlotEmpty, LocInv);

	if (IsSlotEmpty)
	{

	}
	else
	{
	*/
		bool isSlotEmpty = false;
		FInventory TempInventorySlot;

		GetItemInfoAtIndex(StackIndex, isSlotEmpty, TempInventorySlot);
		if (TempInventorySlot.ItemsInfo.ItemcanBeStacked && TempInventorySlot.ItemsInfo.AmountOfItemsInSlot > AmountOfItemsToSplit)
		{
			bool bIsFound = false;
			int32 EmptySlotIndex = SearchEmptySlotIndex(bIsFound);
			if (bIsFound)
			{
				InventorySlots[StackIndex].ItemsInfo.AmountOfItemsInSlot -= AmountOfItemsToSplit;
				//InventorySlots[EmptySlotIndex].MasterItem = InventorySlots[StackIndex].MasterItem;
				InventorySlots[EmptySlotIndex].ItemsInfo = InventorySlots[StackIndex].ItemsInfo;
				//InventorySlots[EmptySlotIndex].ItemsInfo.AmountOfItemsInSlot = AmountOfItemsToSplit;
				bIsSucces = true;
			}
		}

	//}
	OnAmountOfItemsChanged.Broadcast();
	return bIsSucces;
}

bool UProjectXInventoryComponent::AddToIndex(int32 FromIndex, int32 ToIndex)
{
	bool bIsSuccess = false;
	if (InventorySlots[FromIndex].ItemsInfo.ItemName == InventorySlots[ToIndex].ItemsInfo.ItemName && InventorySlots[ToIndex].ItemsInfo.AmountOfItemsInSlot < InventorySlots[ToIndex].ItemsInfo.MaxItemsStackAtSlot && InventorySlots[ToIndex].ItemsInfo.ItemcanBeStacked)
	{
		if (InventorySlots[ToIndex].ItemsInfo.MaxItemsStackAtSlot - GetAmountOfItemsAtIndex(ToIndex) >= GetAmountOfItemsAtIndex(FromIndex))
		{			
			InventorySlots[ToIndex].ItemsInfo.AmountOfItemsInSlot = GetAmountOfItemsAtIndex(FromIndex) + GetAmountOfItemsAtIndex(ToIndex);
			
			InventorySlots[FromIndex] = {};
			
			bIsSuccess = true;
			
		}
		else
		{
			InventorySlots[FromIndex].ItemsInfo.AmountOfItemsInSlot -= InventorySlots[FromIndex].ItemsInfo.MaxItemsStackAtSlot - GetAmountOfItemsAtIndex(ToIndex);
			InventorySlots[ToIndex].ItemsInfo.AmountOfItemsInSlot = InventorySlots[FromIndex].ItemsInfo.MaxItemsStackAtSlot;
			bIsSuccess = true;
		}
		OnAmountOfItemsChanged.Broadcast();
	}
	
	return bIsSuccess;
}

bool UProjectXInventoryComponent::SplitStackToIndex(int32 FromIndex, int32 ToIndex, int32 AmountToSplit)
{
	bool bIsSucces = false;
	bool IsSlotToEmpty;
	bool IsSlotFromEmpty;
	FInventory LocInv;
	GetItemInfoAtIndex(ToIndex, IsSlotToEmpty, LocInv);
	GetItemInfoAtIndex(ToIndex, IsSlotFromEmpty, LocInv);

	if (IsSlotToEmpty && IsSlotFromEmpty && InventorySlots[FromIndex].ItemsInfo.ItemcanBeStacked)
	{
		if (InventorySlots[FromIndex].ItemsInfo.AmountOfItemsInSlot > 1 && InventorySlots[FromIndex].ItemsInfo.AmountOfItemsInSlot > AmountToSplit)
		{
			InventorySlots[FromIndex].ItemsInfo.AmountOfItemsInSlot -= AmountToSplit;
			//InventorySlots[ToIndex].MasterItem = InventorySlots[FromIndex].MasterItem;
			InventorySlots[ToIndex].ItemsInfo = InventorySlots[FromIndex].ItemsInfo;
			InventorySlots[ToIndex].ItemsInfo.AmountOfItemsInSlot = AmountToSplit;
			bIsSucces = true;
			OnAmountOfItemsChanged.Broadcast();
		}
	}

	return bIsSucces;
}

void UProjectXInventoryComponent::DestroyItems(int32 ItemIndex)
{
	bool isSlotEpty = false;
	FInventory TempInv;
	GetItemInfoAtIndex(ItemIndex, isSlotEpty, TempInv);
	
	ChangeCurrentWeight(TempInv, TempInv.ItemsInfo.AmountOfItemsInSlot, false);
	//int32 AmountOfItemsAtIndex = GetAmountOfItemsAtIndex(ItemIndex);
	InventorySlots[ItemIndex] = {};
	InventorySlots[ItemIndex].ItemsInfo.isSlotOccupied = false;

	OnAmountOfItemsChanged.Broadcast();
}

void UProjectXInventoryComponent::InitInventory(TArray<FWeaponSlot> NewWeaponSlotsInfo, TArray<FAmmoSlot> NewAmmoSlotsInfo)
{
	EquipmentSlots.SetNum(4);	
	EquipmentSlots[0].EquipmentInfo.SlotType = EEquipmentSlotType::Bracer;
	EquipmentSlots[1].EquipmentInfo.SlotType = EEquipmentSlotType::Armor;
	EquipmentSlots[2].EquipmentInfo.SlotType = EEquipmentSlotType::BodyKit;
	EquipmentSlots[3].EquipmentInfo.SlotType = EEquipmentSlotType::BackPack;	


}

void UProjectXInventoryComponent::WeaponINIT()
{
	for (int8 i = 0; i < WeaponSlotsInfo.Num(); i++)
	{
		if (WeaponSlotsInfo[i].WeaponClass)
		{
			UProjectXGameInstance* MyGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());
			FInventory ItemInfo;
			MyGI->GetItemInfoByName(WeaponSlotsInfo[i].WeaponName, ItemInfo);

			ChangeCurrentWeight(ItemInfo, 1, true);

			//ChangeCurrentWeight();
		}
		
	}
	
}

void UProjectXInventoryComponent::BodyKitInit(FWeaponInfo WeaponInfo)
{
	if (WeaponInfo.WeaponClass)
	{
		for (int8 f = 0; f < AmmoSlots.Num(); f++)
		{
			if (AmmoSlots[f].WeaponType == WeaponInfo.WeaponType)
			{
				BodyKitAmmoTypeChange(AmmoSlots[f]);
			}
			else
			{
				
			}

		}
	}
		AmmoSlots.Empty();
		for (int8 i = 0; i < WeaponSlotsInfo.Num(); i++)
		{
			if (WeaponSlotsInfo[i].WeaponClass)
			{
				for (int8 j = 0; j < BodyKitAmmoSlotsInfo.Num(); j++)
				{
					if (WeaponSlotsInfo[i].WeaponType == BodyKitAmmoSlotsInfo[j].WeaponType)
						AmmoSlots.Add(BodyKitAmmoSlotsInfo[j]);
				}
			}
		}
	/*  }
	else
	{
		AmmoSlots.Empty();
		for (int8 i = 0; i < WeaponSlotsInfo.Num(); i++)
		{
			if (WeaponSlotsInfo[i].WeaponClass)
			{
				for (int8 j = 0; j < BodyKitAmmoSlotsInfo.Num(); j++)
				{
					if (WeaponSlotsInfo[i].WeaponType == BodyKitAmmoSlotsInfo[j].WeaponType)
						AmmoSlots.Add(BodyKitAmmoSlotsInfo[j]);
				}
			}
		}
	}*/
	OnEquipItem.Broadcast();
	
}





