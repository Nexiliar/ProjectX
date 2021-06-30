// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectX/Items/MasterItem.h"
#include "ProjectX/Game/ProjectXGameInstance.h"


// Sets default values
AMasterItem::AMasterItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ItemInit();

}




// Called when the game starts or when spawned
void AMasterItem::BeginPlay()
{
	Super::BeginPlay();
	//CoreItemInfo = Inventory.InventorySlot.ItemsInfo;

	//ItemInit();
}

// Called every frame
void AMasterItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMasterItem::OnItemUsed(EEquipmentSlotType ItemType)
{
/* 
	
	if (ItemType == EEquipmentSlotType::BodyKit)
	{
		AmmoSlot.WeaponType = EWeaponType::Pistol;
		AmmoSlot.MaxCount = 400;
		AmmoSlot.Count = 100;
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("TEST"));
	}
*/
	switch (ItemType)
	{
	case EEquipmentSlotType::Bracer:
		break;
	case EEquipmentSlotType::BodyKit:
	
		//AmmoSlot.WeaponType = EWeaponType::Pistol;
		//AmmoSlot.MaxCount = 400;
		//AmmoSlot.Count = 100;
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("BodyKitAmmoInitialized"));
	
		break;
	case EEquipmentSlotType::Armor:
		break;
	case EEquipmentSlotType::BackPack:
		break;
	default:
		break;
	}
	
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("BodyKitEquipped"));

}

void AMasterItem::ItemInit(FName Name)
{

	UProjectXGameInstance* MyGI = Cast<UProjectXGameInstance>(GetGameInstance());
	FInventory myInventory;

	if (MyGI)
	{
		
		if (MyGI->GetItemInfoByName(Name, myInventory))
		{
			
			if (myInventory.InventorySlot.MasterItem)
			{
				CurrentInventory = myInventory;
				
			}
		}
	}
	


}

void AMasterItem::ReturnAll(FInventory& Inventory)
{
	Inventory = CurrentInventory;
}

