// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectX/FunctionLibrary/Types.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "MasterItem.generated.h"

USTRUCT(BlueprintType)
struct  FItemsInfoCore
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		FText ItemDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		UTexture2D* ItemIcon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		bool ItemcanBeUsed = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		FText ItemUseText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		bool ItemcanBeStacked = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
		EItemType Itemtype = EItemType::Consumeable;

};

UCLASS()
class PROJECTX_API AMasterItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMasterItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FInventory CurrentInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FItemsInfo CoreItemInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FItemsInfoCore MainItemInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FAmmoSlot AmmoSlot;
	
	FName lol;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EEquipmentSlotType SlotType;

	UFUNCTION(BlueprintCallable)
	void OnItemUsed(EEquipmentSlotType ItemType);
	
	UFUNCTION(BlueprintCallable)
	void ItemInit(FName Name);

	UFUNCTION(BlueprintCallable)
		void ReturnAll(FInventory& Inventory);
};
