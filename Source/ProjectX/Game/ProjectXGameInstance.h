// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ProjectX/FunctionLibrary/Types.h"
#include "Engine/DataTable.h"
#include "ProjectX/Weapons/WeaponDefault.h"
#include "ProjectXGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTX_API UProjectXGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	//table
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " WeaponSetting ")
		UDataTable* WeaponInfoTable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " Inventory ")
		UDataTable* InventoryTable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " ItemDrop ")
		UDataTable* DropItemInfotable = nullptr;

	UFUNCTION(BlueprintCallable)
		bool GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo);
	UFUNCTION(BlueprintCallable)
		bool GetDropItemInfoByName(FName NameItem, FDropItem& OutInfo);

	UFUNCTION(BlueprintCallable)
		bool GetItemInfoByName(FName Name, FInventory& OutInventoryInfo);
};
