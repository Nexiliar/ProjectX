// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectX/Game/ProjectXGameInstance.h"

bool UProjectXGameInstance::GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo)
{
	bool bIsFind = false;
	FWeaponInfo* WeaponInfoRow;

	if (WeaponInfoTable)
	{
		WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(NameWeapon, "", false);
		if (WeaponInfoRow)
		{
			bIsFind = true;
			OutInfo = *WeaponInfoRow;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UProjectXGameInstance::GetWeaponInfoByName - WeaponTable -NULL"));
	}

	return bIsFind;
}

bool UProjectXGameInstance::GetDropItemInfoByName(FName NameItem, FDropItem& OutInfo)
{
	bool bIsFind = false;
	if (DropItemInfotable)
	{
		FDropItem* DropItemInfoRow;
		TArray<FName> RowNames = DropItemInfotable->GetRowNames();
		
		int8 i = 0;
		while(i<RowNames.Num() && !bIsFind)
		{
			DropItemInfoRow = DropItemInfotable->FindRow<FDropItem>(RowNames[i], "");
			if (DropItemInfoRow->WeaponInfo.NameItem == NameItem)
			{
				OutInfo = (*DropItemInfoRow);
				bIsFind = true;
			}
			i++;
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTPSGameInstance::GetWeaponInfoByName - DropItemInfotable -NULL"));
	}
	return bIsFind;
}

bool UProjectXGameInstance::GetItemInfoByName(FName Name, FInventory& OutInventoryInfo)
{
	
	bool bIsFind = false;
	FInventory* Inventory;
	if (InventoryTable)
	{
		Inventory = InventoryTable->FindRow<FInventory>(Name, "", false);
		if (Inventory)
		{
			bIsFind = true;
			OutInventoryInfo = *Inventory;
			
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectXGameinstance::GetItemInfoByName - InventoryTable -NULL"));
	}
	return bIsFind;
}
