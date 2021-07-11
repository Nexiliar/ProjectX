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


}


// Called every frame
void UProjectXInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
 
bool UProjectXInventoryComponent::SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAddicionalWeaponInfo OldInfo, bool bIsForward)
{
	
	bool bIsSUcces = false;
	int8 CorrectIndex = ChangeToIndex;
	if (ChangeToIndex > WeaponSlots.Num() - 1)
		CorrectIndex = 0;
	else
		if (ChangeToIndex < 0)
			CorrectIndex = WeaponSlots.Num() - 1;
			
	FName NewIdWeapon;
	FAddicionalWeaponInfo NewAdditionalInfo;
	int32 NewCurrentIndex = 0;
	if (WeaponSlots.IsValidIndex(CorrectIndex))
	{
		if (!WeaponSlots[CorrectIndex].NameItem.IsNone())
		{
			if (WeaponSlots[CorrectIndex].AdditionalInfo.Round > 0)
			{
				//У оружия есть патроны, поменять оружие
				bIsSUcces = true;
			}
			else
			{
				UProjectXGameInstance* myGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());
				if (myGI)
				{
					//Проверяем ammoslots для оружия
					FWeaponInfo myInfo;
					myGI->GetWeaponInfoByName(WeaponSlots[CorrectIndex].NameItem, myInfo);
					bool bIsFind = false;
					int8 j = 0;
					while (j < AmmoSlots.Num() && !bIsFind)
					{
						if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Count > 0)
						{
							bIsSUcces = true;
							bIsFind = true;
						}
						j++;
					}
				}
			}
			if (bIsSUcces)
			{
				NewCurrentIndex = CorrectIndex;
				NewIdWeapon = WeaponSlots[CorrectIndex].NameItem;
				NewAdditionalInfo = WeaponSlots[CorrectIndex].AdditionalInfo;
			}
		}
	}

	if (!bIsSUcces)
	{
		if (bIsForward)
		{
			int8 iteration = 0;
			int8 Seconditeration = 0;
			while (iteration < WeaponSlots.Num() && !bIsSUcces)
			{
				iteration++;
				int8 tmpIndex = ChangeToIndex + iteration;
				if (WeaponSlots.IsValidIndex(tmpIndex))
				{
					if (!WeaponSlots[tmpIndex].NameItem.IsNone())
					{
						if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
						{
							//WeaponGood
							bIsSUcces = true;
							NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
							NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
							NewCurrentIndex = tmpIndex;
						}
						else
						{
							FWeaponInfo myInfo;
							UProjectXGameInstance* myGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());

							myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

							bool bIsFind = false;
							int8 j = 0;
							while (j < AmmoSlots.Num() && !bIsFind)
							{
								if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Count > 0)
								{
									//WeaponGood
									bIsSUcces = true;
									NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
									NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
									NewCurrentIndex = tmpIndex;
									bIsFind = true;
								}
								j++;
							}
						}
					}
				}
				else
				{
					//go to end of right of array weapon slots
					if (OldIndex != Seconditeration)
					{
						if (WeaponSlots.IsValidIndex(Seconditeration))
						{
							if (!WeaponSlots[Seconditeration].NameItem.IsNone())
							{
								if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
								{
									//WeaponGood
									bIsSUcces = true;
									NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
									NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
									NewCurrentIndex = Seconditeration;
								}
								else
								{
									FWeaponInfo myInfo;
									UProjectXGameInstance* myGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());

									myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Count > 0)
										{
											//WeaponGood
											bIsSUcces = true;
											NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
											NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
											NewCurrentIndex = Seconditeration;
											bIsFind = true;
										}
										j++;
									}
								}
							}
						}
					}
					else
					{
						//go to same weapon when start
						if (WeaponSlots.IsValidIndex(Seconditeration))
						{
							if (!WeaponSlots[Seconditeration].NameItem.IsNone())
							{
								if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
								{
									//WeaponGood, it same weapon do nothing
								}
								else
								{
									FWeaponInfo myInfo;
									UProjectXGameInstance* myGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());

									myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
										{
											if (AmmoSlots[j].Count > 0)
											{
												//WeaponGood, it same weapon do nothing
											}
											else
											{
												//Not find weapon with amm need init Pistol with infinity ammo
												UE_LOG(LogTemp, Error, TEXT("UTPSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
											}
										}
										j++;
									}
								}
							}
						}
					}
					Seconditeration++;
				}
			}
		}
		else
		{
			int8 iteration = 0;
			int8 Seconditeration = WeaponSlots.Num() - 1;
			while (iteration < WeaponSlots.Num() && !bIsSUcces)
			{
				iteration++;
				int8 tmpIndex = ChangeToIndex - iteration;
				if (WeaponSlots.IsValidIndex(tmpIndex))
				{
					if (!WeaponSlots[tmpIndex].NameItem.IsNone())
					{
						if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
						{
							//WeaponGood
							bIsSUcces = true;
							NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
							NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
							NewCurrentIndex = tmpIndex;
						}
						else
						{
							FWeaponInfo myInfo;
							UProjectXGameInstance* myGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());

							myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

							bool bIsFind = false;
							int8 j = 0;
							while (j < AmmoSlots.Num() && !bIsFind)
							{
								if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Count > 0)
								{
									//WeaponGood
									bIsSUcces = true;
									NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
									NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
									NewCurrentIndex = tmpIndex;
									bIsFind = true;
								}
								j++;
							}
						}
					}
				}
				else
				{
					//go to end of LEFT of array weapon slots
					if (OldIndex != Seconditeration)
					{
						if (WeaponSlots.IsValidIndex(Seconditeration))
						{
							if (!WeaponSlots[Seconditeration].NameItem.IsNone())
							{
								if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
								{
									//WeaponGood
									bIsSUcces = true;
									NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
									NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
									NewCurrentIndex = Seconditeration;
								}
								else
								{
									FWeaponInfo myInfo;
									UProjectXGameInstance* myGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());

									myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Count > 0)
										{
											//WeaponGood
											bIsSUcces = true;
											NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
											NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
											NewCurrentIndex = Seconditeration;
											bIsFind = true;
										}
										j++;
									}
								}
							}
						}
					}
					else
					{
						//go to same weapon when start
						if (WeaponSlots.IsValidIndex(Seconditeration))
						{
							if (!WeaponSlots[Seconditeration].NameItem.IsNone())
							{
								if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
								{
									//WeaponGood, it same weapon do nothing
								}
								else
								{
									FWeaponInfo myInfo;
									UProjectXGameInstance* myGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());

									myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
										{
											if (AmmoSlots[j].Count > 0)
											{
												//WeaponGood, it same weapon do nothing
											}
											else
											{
												//Not find weapon with amm need init Pistol with infinity ammo
												UE_LOG(LogTemp, Error, TEXT("UTPSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
											}
										}
										j++;
									}
								}
							}
						}
					}
					Seconditeration--;
				}
			}
		}
	}
	if (bIsSUcces)
	{
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo, NewCurrentIndex);
	}
	return bIsSUcces;
}

FAddicionalWeaponInfo UProjectXInventoryComponent::GetAdditionalInfoWeapon(int32 IndexWeapon)
{
	FAddicionalWeaponInfo result;
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			if (i == IndexWeapon)
			{
				result = WeaponSlots[i].AdditionalInfo;
				bIsFind = true;
			}
		i++;
		}
		if (!bIsFind)
		{
			UE_LOG(LogTemp, Warning, TEXT("ProjectXInventoryComponent::SetAdditionalInfoWeapon = No found weapon with index - % d"), IndexWeapon);
		}

	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ProjectXInventoryComponent::SetAdditionalInfoWeapon = Not correct index weapon - % d"), IndexWeapon);

	return result;
}

int32 UProjectXInventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
	int32 result = -1;
	int8 i = 0;
	bool bIsFind = false;
	while (i < WeaponSlots.Num() && !bIsFind)
	{
		if (WeaponSlots[i].NameItem == IdWeaponName)
		{
			bIsFind = true;
			result = i;
		}
		i++;
	}
	return result;
}

FName UProjectXInventoryComponent::GetWeaponNameBySlotIndex(int32 IndexSlot)
{
	FName result;
	if (WeaponSlots.IsValidIndex(IndexSlot))
	{
		result = WeaponSlots[IndexSlot].NameItem;
	}

	return result;
}

void UProjectXInventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAddicionalWeaponInfo Newinfo)
{
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			if (i == IndexWeapon)
			{
				WeaponSlots[i].AdditionalInfo = Newinfo;
				bIsFind = true;
				OnWeaponAdditionalInfoChange.Broadcast(IndexWeapon,Newinfo);
			}
			i++;
		}
		if (!bIsFind)
		{
			UE_LOG(LogTemp, Warning, TEXT("ProjectXInventoryComponent::SetAdditionalInfoWeapon = No found weapon with index - % d"), IndexWeapon);
		}

	}
	else
		UE_LOG(LogTemp, Warning, TEXT("ProjectXInventoryComponent::SetAdditionalInfoWeapon = Not correct index weapon - % d"), IndexWeapon);



}



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
}

FWeaponInfo UProjectXInventoryComponent::GetWeaponInfo(int32 SlotIndex)
{
	//MainWeaponInfo = WeaponInfo; 
	FWeaponInfo WeaponInfoAtIndex= WeaponSlotsInfo[SlotIndex];

	return WeaponInfoAtIndex;
}

bool UProjectXInventoryComponent::CheckAmmoForWeapon(EWeaponType TypeWeapon, int32 &AviableAmmoForWeapon)
{
	AviableAmmoForWeapon = 0;
	bool bIsFind = false;
	int8 i = 0;
	while (i< AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon )
		{
			bIsFind = true;
			AviableAmmoForWeapon = AmmoSlots[i].Count;
			if (AmmoSlots[i].Count > 0)
				return true;
		}
			
		i++;
	}

	OnWeaponAmmoEmpty.Broadcast(TypeWeapon);

	return false;
}

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

bool UProjectXInventoryComponent::CheckCanTakeWeapon(int32 &FreeSlot)
{
	bool bIsFreeslot = false;
	int8 i = 0;
	while (i < WeaponSlots.Num() && !bIsFreeslot)
	{
		if (WeaponSlots[i].NameItem.IsNone())
		{
			bIsFreeslot = true;
			FreeSlot = i;
		}
		i++;
	}
	return bIsFreeslot;
}

bool UProjectXInventoryComponent::SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot, int32 CurrentIndexWeaponChar, FDropItem &DropItemInfo)
{
	bool result = false;

	if (WeaponSlots.IsValidIndex(IndexSlot) && GetDropItemInfoFromInventory(IndexSlot, DropItemInfo))
	{
		WeaponSlots[IndexSlot] = NewWeapon;
		
		SwitchWeaponToIndex(CurrentIndexWeaponChar,-1,NewWeapon.AdditionalInfo,true);
		
		

		OnUpdateWeaponSlot.Broadcast(IndexSlot, NewWeapon);
		result = true;
		
	
	}
	OnAmountOfItemsChanged.Broadcast();
	return result;
}

void UProjectXInventoryComponent::TryGetWeaponToInventory(FWeaponSlot NewWeapon)
{
	int32 indexSlot = -1;
	if (CheckCanTakeWeapon(indexSlot))
	{
		if (WeaponSlots.IsValidIndex(indexSlot))
		{
			WeaponSlots[indexSlot] = NewWeapon;

			OnUpdateWeaponSlot.Broadcast(indexSlot, NewWeapon);
			
		}
	}
	OnAmountOfItemsChanged.Broadcast();
}

bool UProjectXInventoryComponent::GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo)
{
	bool result = false;
	
	FName DropItemName = GetWeaponNameBySlotIndex(IndexSlot);
	
	UProjectXGameInstance* myGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());
	if (myGI)
	{
		
		result = myGI->GetDropItemInfoByName(DropItemName, DropItemInfo);
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			DropItemInfo.WeaponInfo.AdditionalInfo = WeaponSlots[IndexSlot].AdditionalInfo;
		}
		
	}

	return result;
}




/* 
bool UProjectXInventoryComponent::EquipItem(int32 SlotIndex)
{
	bool bIsFind = false;
	int8 i = 0;
	bool isEquipSuccess = false;
	
	while (i < EquipmentSlots.Num() && !bIsFind)
	{		
		if (EquipmentSlots[i].EquipmentInfo.SlotType == InventorySlots[SlotIndex].EquipmentInfo.SlotType)
		{
			EquipmentSlots[i] = InventorySlots[SlotIndex];	
			InventorySlots[SlotIndex] = {};

			switch (EquipmentSlots[i].EquipmentInfo.SlotType)
			{
				case EEquipmentSlotType::Bracer:

					break;
				case EEquipmentSlotType::BodyKit:
					AmmoSlots.SetNum(4);
					AmmoSlots[0].WeaponType = EWeaponType::Pistol;
					AmmoSlots[0].MaxCount = 200;
					AmmoSlots[0].Count = 100;
					AmmoSlots[1].WeaponType = EWeaponType::ShotGunType;
					AmmoSlots[1].MaxCount = 80;
					AmmoSlots[1].Count = 20;
					AmmoSlots[2].WeaponType = EWeaponType::Rifle;
					AmmoSlots[2].MaxCount = 300;
					AmmoSlots[2].Count = 150;
					AmmoSlots[3].WeaponType = EWeaponType::GrenadeLauncher;
					AmmoSlots[3].MaxCount = 50;
					AmmoSlots[3].Count = 25;
					OnEquipItem.Broadcast();
					isBodyKitEquiped = true;
					//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("BodyKitAmmoInitialized"));

					break;
				case EEquipmentSlotType::Armor:
					isArmorEquiped = true;
					break;
				case EEquipmentSlotType::BackPack:
			//		EquipBackPack(EquipmentSlots[i], isEquipSuccess);
					break;
				default:
					break;
			}
			//InventorySlots[SlotIndex].MasterItem = NULL;
			InventorySlots[SlotIndex].ItemsInfo = {};
			//InventorySlots[SlotIndex]. ItemsInfo.AmountOfItemsInSlot = 0;
			bIsFind = true;
			//isBodyKitEquiped = true;
			OnAmountOfItemsChanged.Broadcast();

			/* Временно, чекнуть как работает свич, если все ок, убрать логику
			if (InventorySlots[SlotIndex].ItemsInfo.EquipmentInfo.SlotType == EEquipmentSlotType::BodyKit)
			{
				AmmoSlots.SetNum(4);
				AmmoSlots[0].WeaponType = EWeaponType::Pistol;
				AmmoSlots[0].MaxCount = 200;
				AmmoSlots[0].Count = 100;
				AmmoSlots[1].WeaponType = EWeaponType::ShotGunType;
				AmmoSlots[1].MaxCount = 80;
				AmmoSlots[1].Count = 20;
				AmmoSlots[2].WeaponType = EWeaponType::Rifle;
				AmmoSlots[2].MaxCount = 300;
				AmmoSlots[2].Count = 150;
				AmmoSlots[3].WeaponType = EWeaponType::GrenadeLauncher;
				AmmoSlots[3].MaxCount = 50;
				AmmoSlots[3].Count = 25;
				OnEquipItem.Broadcast();
				isBodyKitEquiped = true;
			}
			if (InventorySlots[SlotIndex].ItemsInfo.EquipmentInfo.SlotType == EEquipmentSlotType::BackPack)
			{
				AmountOfInventorySlots = 20;
				isBackPackEquiped = true;
				InventorySlots.SetNum(AmountOfInventorySlots);
				OnEquipItem.Broadcast();
			}
			if (InventorySlots[SlotIndex].ItemsInfo.EquipmentInfo.SlotType == EEquipmentSlotType::Armor)
			{
				isArmorEquiped = true;
			}
			InventorySlots[SlotIndex].MasterItem = NULL;
			InventorySlots[SlotIndex].ItemsInfo = {};
			InventorySlots[SlotIndex].ItemsInfo.AmountOfItemsInSlot = 0;
			bIsFind = true;
			isBodyKitEquiped = true;
			OnAmountOfItemsChanged.Broadcast();
			 */
		//}
		//i++;
	//}

	//return bIsFind;
//}

bool UProjectXInventoryComponent::UnequipItem(int32 SlotIndex, FInventory& IventorySlotInfo)
{
	bool bIsFind = false;
	if (EquipmentSlots[SlotIndex].EquipmentInfo.SlotType == EEquipmentSlotType::BackPack)
	{
		UnequipBackPack(SlotIndex);
		isBackPackEquiped = false;
		InventorySlots.Empty();
		EquipmentSlots[SlotIndex] = {};
		EquipmentSlots[SlotIndex].EquipmentInfo.SlotType = EEquipmentSlotType::BackPack;
		UE_LOG(LogTemp, Warning, TEXT(" UnEquipSucces"));
		bIsFind = true;
		OnUnEquipItem.Broadcast();
	}
	
	/* 
	int8 i = 0;
	bool bIsFind = false;
	while (i < InventorySlots.Num() && !bIsFind)
	{
		FInventory TempInv;
		bool IsSlotEmpty = false;
		GetItemInfoAtIndex(i, IsSlotEmpty, TempInv);
		if (IsSlotEmpty)
		{
			switch (EquipmentSlots[i].EquipmentInfo.SlotType)
			{
			case EEquipmentSlotType::Bracer:

				break;
			case EEquipmentSlotType::BodyKit:

				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("BodyKitAmmoInitialized"));

				break;
			case EEquipmentSlotType::Armor:
				isArmorEquiped = true;
				break;
			case EEquipmentSlotType::BackPack:
				//EquipBackPack(EquipmentSlots[i], isEquipSuccess);
				break;
			default:
				break;
			}
		}*/
		/*
			if (isBodyKitEquiped && EquipmentSlots[SlotIndex].EquipmentInfo.SlotType == EEquipmentSlotType::BodyKit)
			{
				//AmmoSlots.SetNum(4);
				AmmoSlots[0].WeaponType = EWeaponType::Pistol;
				AmmoSlots[0].MaxCount = 0;
				AmmoSlots[0].Count = 0;
				AmmoSlots[1].WeaponType = EWeaponType::ShotGunType;
				AmmoSlots[1].MaxCount = 0;
				AmmoSlots[1].Count = 0;
				AmmoSlots[2].WeaponType = EWeaponType::Rifle;
				AmmoSlots[2].MaxCount = 0;
				AmmoSlots[2].Count = 0;
				AmmoSlots[3].WeaponType = EWeaponType::GrenadeLauncher;
				AmmoSlots[3].MaxCount = 0;
				AmmoSlots[3].Count = 0;
				OnEquipItem.Broadcast();
				isBodyKitEquiped = false;

			}
			if (isArmorEquiped && EquipmentSlots[SlotIndex].EquipmentInfo.SlotType == EEquipmentSlotType::Armor)
			{
				isArmorEquiped = false;
			}

			ItemInfo = EquipmentSlots[SlotIndex].ItemsInfo;
			//InventorySlots[i].MasterItem = EquipmentSlots[SlotIndex].MasterItem;
			InventorySlots[i].ItemsInfo = EquipmentSlots[SlotIndex].ItemsInfo;
			
			//EquipmentSlots.RemoveAt(SlotIndex);  
			//EquipmentSlots[SlotIndex].MasterItem = NULL;
			
			EquipmentSlots[SlotIndex].ItemsInfo = {};
			//EquipmentSlots[SlotIndex].ItemsInfo.ItemName = " ";
			
			bIsFind = true;
			OnAmountOfItemsChanged.Broadcast();			 
			
		}
		 */	

	//	i++;
	//}
/* 
	if (isBackPackEquiped)
	{

		AmountOfInventorySlots = 4;
		isBackPackEquiped = false;
		InventorySlots.SetNum(AmountOfInventorySlots);
		OnUnEquipItem.Broadcast();
	}
	*/

	return bIsFind;
}
//Check for double in pickupactor DELIF NOT USSED
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
	OnEquipItem.Broadcast();	
}

bool UProjectXInventoryComponent::CheckWeaponSlotEmpty(int32 SlotIndex, bool IsWeapon)
{
	bool isSlotEmpty;

		if (WeaponSlots[SlotIndex].NameItem == "None")
		{
			isSlotEmpty = true;
		}
		else
		{
			isSlotEmpty = false;
		}

	return isSlotEmpty;
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
		DropedBackPack->ItemInit(EquipmentSlots[SlotIndex].ItemsInfo.ItemName, false, InventorySlots);
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
			}
			else
			{
			
				
				InventorySlots[ItemIndex].ItemsInfo.AmountOfItemsInSlot = InventorySlots[ItemIndex].ItemsInfo.AmountOfItemsInSlot + AmountOfItems;
				InventorySlots[ItemIndex].ItemsInfo.isSlotOccupied = true;
				InventorySlots[ItemIndex].EquipmentInfo = InventoryInfo.EquipmentInfo;
				//InventorySlots[ItemIndex].ItemsInfo = LocalInventoryInit.ItemsInfo;
				bIsSuccess = true;
				RestItems = 0;
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
			if(CurrentAmount >1)
			{			
				//for (int8 LocalAmountOfItems = AmountOfItems; LocalAmountOfItems > 1; LocalAmountOfItems--)
				for(CurrentAmount; CurrentAmount>1; CurrentAmount--)
				{
					AddItem(InventoryInfo, 1, RestItems);
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
		if (InventorySlots[ItemIndex].ItemsInfo.Itemtype == EItemType::Equipment)
		{
			//EquipItem(ItemIndex);
			bIsSucces = true;				
		}
		else
		{
		
			if (AmountOfItems >= GetAmountOfItemsAtIndex(ItemIndex))
			{
				InventorySlots[ItemIndex].ItemsInfo = {}; // .AmountOfItemsInSlot = 0;
		//		InventorySlots[ItemIndex].MasterItem = NULL;
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

	}
	
	return bIsSucces;
}

bool UProjectXInventoryComponent::SwapItemsWithIndex(int32 FirstItemIndex, int32 SecondItemIndex, bool IsWeapon)
{
	bool bIsSucces = false;	
	if (IsWeapon)
	{
				FWeaponSlot TempWeaponStorage = WeaponSlots[SecondItemIndex];
				WeaponSlots[SecondItemIndex] = WeaponSlots[FirstItemIndex];
				WeaponSlots[FirstItemIndex] = TempWeaponStorage;
				bIsSucces = true;
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
	//int32 AmountOfItemsAtIndex = GetAmountOfItemsAtIndex(ItemIndex);
	InventorySlots[ItemIndex] = {};
	InventorySlots[ItemIndex].ItemsInfo.isSlotOccupied = false;
	OnAmountOfItemsChanged.Broadcast();
}

void UProjectXInventoryComponent::InitInventory(TArray<FWeaponSlot> NewWeaponSlotsInfo, TArray<FAmmoSlot> NewAmmoSlotsInfo)
{
	WeaponSlots = NewWeaponSlotsInfo;
	AmmoSlots = NewAmmoSlotsInfo;
	//AmountOfInventorySlots = InventorySlotsByCharacterStats;
	//Задаем количество слотов в инвентаре
	//InventorySlots.SetNum(AmountOfInventorySlots);
	//if (InventorySlots.IsValidIndex(AmountOfInventorySlots))
		//UE_LOG(LogTemp, Error, TEXT("InventorySLots.SetNum = AmountOfInventorySLots and indexisvalid"));
//	OnInventoryWidgetCreate.Broadcast();

	//EquipmentSlots = new TArray<FInventory>[4];
	EquipmentSlots.SetNum(4);	
	EquipmentSlots[0].EquipmentInfo.SlotType = EEquipmentSlotType::Bracer;
	EquipmentSlots[1].EquipmentInfo.SlotType = EEquipmentSlotType::Armor;
	EquipmentSlots[2].EquipmentInfo.SlotType = EEquipmentSlotType::BodyKit;
	EquipmentSlots[3].EquipmentInfo.SlotType = EEquipmentSlotType::BackPack;
	


	for (int8 i = 0; i < WeaponSlots.Num(); i++)
	{
		UProjectXGameInstance* myGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());
		if (myGI)
		{
			if (!WeaponSlots[i].NameItem.IsNone())
			{
				//FWeaponInfo  Info;
				//if (myGI->GetWeaponInfoByName(WeaponSlots[i].NameItem, Info))
					//WeaponSlots[i].AdditionalInfo.Round = Info.MaxRound;

			}

		}
	}

	MaxSlotWeapon = WeaponSlots.Num();

	if (WeaponSlots.IsValidIndex(0))
	{
		if (!WeaponSlots[0].NameItem.IsNone())
			OnSwitchWeapon.Broadcast(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo, 0);
	}

}





