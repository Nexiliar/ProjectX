// Fill out your copyright notice in the Description page of Project Settings.

#include "UObject/Class.h"
#include "DrawDebugHelpers.h"
#include "ProjectX/Items/PickUpActor.h"

// Sets default values
APickUpActor::APickUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshForItem"));
	StaticMesh->SetGenerateOverlapEvents(true);
	StaticMesh->SetSimulatePhysics(true);	
	StaticMesh->SetEnableGravity(true);
	StaticMesh->SetWorldScale3D(ScaleForMesh);
	StaticMesh->SetCollisionProfileName(TEXT("Query and Physics"));
	RootComponent = StaticMesh;

	

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	BoxComponent->SetupAttachment(RootComponent);


	CloseToPickUp = CreateDefaultSubobject<UMaterialInstance>(TEXT("On Material"));
	GetOutOfWeapon= CreateDefaultSubobject<UMaterialInstance>(TEXT("Off Material"));;
}

// Called when the game starts or when spawned
void APickUpActor::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APickUpActor::CollisionBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &APickUpActor::CollisionBoxEndOverlap);

	StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &APickUpActor::StaticMeshBeginOverlap);

	//�������������� ���������� Character
	Character = Cast<AProjectXCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	
	
	
	ItemInit(NameOfTheItem, isNewItem, InventorySlots,ItemCFG);
	
}

// Called every frame
void APickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}




void APickUpActor::CollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Character = Cast<AProjectXCharacter>(OtherActor);
		
	GetWorld()->GetTimerManager().SetTimer(Timer_LineTrace, this, &APickUpActor::LineTracingCheck, 0.1f, true);	
}

void APickUpActor::CollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
	GetWorld()->GetTimerManager().ClearTimer(Timer_LineTrace);
	isOverlapping = false;
	OverlapStart_BP(isOverlapping);
}

void APickUpActor::LineTracingCheck()
{
	FHitResult Hit;
	FVector End;
	FVector Start = GetActorLocation();	
	if (Character)
	{
		End = Character->GetMesh()->GetSocketLocation("Spine_Socket");
	}

	FVector Result = Start - End;
	
	//UE_LOG(LogTemp, Warning, TEXT("Vector: Size = %f"), Result.Size());
	

	if (Result.Size()<=150.0f)
	{
		if (!isOverlapping)
		{
			if (Character && Character->InventoryComponent)
			{
				Character->OnIterractButtonPressed.AddDynamic(this, &APickUpActor::TryToPickUpItem);
				isOverlapping = true;
				OverlapStart_BP(isOverlapping);
			}
		}

	}
	else
	{
		if (isOverlapping)
		{
			if (Character)
				Character->OnIterractButtonPressed.Clear();
			isOverlapping = false;
			OverlapStart_BP(isOverlapping);
		}
	}
}

void APickUpActor::StaticMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Character = Cast<AProjectXCharacter>(OtherActor);

	//if (Character && Character->InventoryComponent)
	//{
	//	Character->OnIterractButtonPressed.AddDynamic(this, &APickUpActor::TryToPickUpItem);
	//	isOverlapping = true;
	//	OverlapStart_BP(isOverlapping);
	//}

	if (ItemCFG.ItemsInfo.Itemtype != EItemType::Equipment)
	{ 
		//Character = Cast<AProjectXCharacter>(OtherActor);
			if (Character && Character->InventoryComponent && Character->InventoryComponent->InventorySlots.Num()>1)
			{
				
					int32 RestItems;
					bool EmptySlotisFind = false;
					bool FreeStackisFind = false;
					Character->InventoryComponent->SearchEmptySlotIndex(EmptySlotisFind);
			
					if (EmptySlotisFind)
					{						
							if (Character->InventoryComponent->AddItem(ItemCFG, AmountItemsTospawn, RestItems))
							{
								if (RestItems > 0)
								{
									AmountItemsTospawn = RestItems;
								}
								else
								{
									this->Destroy();
									isOverlapping = false;
									OverlapStart_BP(isOverlapping);
								}							
							}					
					}
					else
					{
						if (Character->InventoryComponent->SearchFreeStack(ItemCFG.ItemsInfo, FreeStackisFind))
						{
							if (Character->InventoryComponent->AddItem(ItemCFG, AmountItemsTospawn, RestItems))
							{
								if (RestItems > 0)
								{
									AmountItemsTospawn = RestItems;
								}
								else
								{
									this->Destroy();
									isOverlapping = false;
									OverlapStart_BP(isOverlapping);
								}
							}
						}
					}
			}
	}
}

void APickUpActor::ItemInit(FName ItemName, bool IsAnItemNew, TArray<FInventory> InfoInSlotsOfBackPack, FInventory DropedItem)
{
	NameOfTheItem = ItemName;
	FInventory ItemInfo;
	if(DropedItem.ItemsInfo.ItemName == "None")
	{
		UProjectXGameInstance* MyGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());
		
		if (MyGI)
		{
			MyGI->GetItemInfoByName(ItemName, ItemInfo);
		}
	}
	else
	{
		ItemInfo = DropedItem;
	}

	if (ItemInfo.ItemsInfo.ItemName == "None")
	{
	}
	else
	{
		ItemCFG = ItemInfo;
		EEquipmentSlotType SlotType = ItemCFG.EquipmentInfo.SlotType;
		StaticMesh->SetStaticMesh(ItemCFG.ItemsInfo.ItemMesh);
		switch (SlotType)
		{
		case EEquipmentSlotType::None:
			break;
		case EEquipmentSlotType::Weapon:
			InitWeapon(IsAnItemNew, DropedItem);
			break;
		case EEquipmentSlotType::Bracer:
			InitBracer(IsAnItemNew, DropedItem);
			break;
		case EEquipmentSlotType::BodyKit:
			InitBodyKit(IsAnItemNew);
			break;
		case EEquipmentSlotType::Armor:
			InitArmor(IsAnItemNew);
			break;
		case EEquipmentSlotType::BackPack:		
			InitBackPack(ItemCFG, IsAnItemNew,InfoInSlotsOfBackPack);
			break;
		default:
			break;
		}
	}
	

}

void APickUpActor::CheckRarity()
{
	int8 Random = FMath::RandRange(1, 110);
	if (Random <= 60)
	{
		ItemCFG.EquipmentInfo.ItemRarity = ERarity::Common;
	}
	else if (Random >= 61 && Random <= 80)
	{
		ItemCFG.EquipmentInfo.ItemRarity = ERarity::Uncommon;
	}
	else if (Random >= 80 && Random <= 91)
	{
		ItemCFG.EquipmentInfo.ItemRarity = ERarity::Rare;
	}
	else if (Random >= 92 && Random <= 103)
	{
		ItemCFG.EquipmentInfo.ItemRarity = ERarity::Epic;
	}
	else if (Random >= 104 && Random <= 110)
	{
		ItemCFG.EquipmentInfo.ItemRarity = ERarity::Legendary;
	}

}

void APickUpActor::InitBackPack(FInventory CurrentItemInfo, bool ItemIsNew, TArray<FInventory> InfoInSlotsOfBackPack)
{

	if (ItemIsNew)
	{		
		CheckRarity();
		ERarity ItemRarity = ItemCFG.EquipmentInfo.ItemRarity;
		//UGameplayStatics::RandomBoolWithWeight();
		if (CurrentItemInfo.ItemsInfo.ItemName == "LightBackPack")
		{
			SetBackPackSlotsAmount(6);
		}
		else if (CurrentItemInfo.ItemsInfo.ItemName == "MediumBackpack")
		{
			SetBackPackSlotsAmount(10);
		}
		else if (CurrentItemInfo.ItemsInfo.ItemName == "HugeBackPack")
		{
			SetBackPackSlotsAmount(15);
		}

		//ERarity ItemRarity = CurrentItemInfo.EquipmentInfo.ItemRarity;
		switch (ItemRarity)
		{
		case ERarity::None:
			break;
		case ERarity::Common:
			SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 1);
			break;
		case ERarity::Uncommon:
			SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 2);
			break;
		case ERarity::Rare:
			SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 5);
			break;
		case ERarity::Epic:
			SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 10);
			break;
		case ERarity::Legendary:
			SetBackPackSlotsAmount(GetBackPackSlotsAmount() + 20);
			break;
		default:
			break;
		}
		InventorySlots.SetNum(GetBackPackSlotsAmount());
		
	}
	else
	{
		//ItemCFG = CurrentItemInfo;
		InventorySlots.SetNum(GetBackPackSlotsAmount());
		InventorySlots = InfoInSlotsOfBackPack;
		 
		for (int8 i = 0; i < GetBackPackSlotsAmount(); i++)
		{			
			if (InventorySlots[i].ItemsInfo.ItemName == "None")
			{
				InventorySlots[i].ItemsInfo.isSlotOccupied = false;
			}	
		}			
	}
}

void APickUpActor::TryToPickUpItem()
{
	Character = Cast<AProjectXCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
	//int32 FreeSlotIndex = 0;
	//bool isFreeSlot = Character->InventoryComponent->CheckCanTakeWeapon(FreeSlotIndex);
	if (isOverlapping)
	{
		EEquipmentSlotType SlotType = ItemCFG.EquipmentInfo.SlotType;
		switch (SlotType)
		{
		case EEquipmentSlotType::None:
			break;
		case EEquipmentSlotType::Weapon:
			if (EquipWeapon())
				this->Destroy();
			break;
		case EEquipmentSlotType::Bracer:
			if (EquipBracer())
				this->Destroy();
				
			break;
		case EEquipmentSlotType::BodyKit:
			if (EquipBodyKit())
				this->Destroy();
			break;
		case EEquipmentSlotType::Armor:
			if(EquipArmor())
			this->Destroy();
			break;
		case EEquipmentSlotType::BackPack:
			
			if(EquipBackPack())
			this->Destroy();
			break;
		default:
			break;
		}
		isOverlapping = false;
		OverlapStart_BP(isOverlapping);
	}	
}



void APickUpActor::DestroyItem(int32 ItemIndex)
{
	InventorySlots[ItemIndex] = {};
}



int32 APickUpActor::GetBackPackSlotsAmount()
{
	return BackPackSlotAmount;
}
void APickUpActor::SetBackPackSlotsAmount(int32 AmountOfSlots)
{
	BackPackSlotAmount = AmountOfSlots;
}
bool APickUpActor::EquipBackPack()
{
	bool bIsBackPackEquipSuccess = false;
	UProjectXInventoryComponent* myInventory = Cast<UProjectXInventoryComponent>(Character->GetComponentByClass(UProjectXInventoryComponent::StaticClass()));
	myInventory->EquipBackPack(ItemCFG, InventorySlots,GetBackPackSlotsAmount(), bIsBackPackEquipSuccess);
	return bIsBackPackEquipSuccess;
}
void APickUpActor::InitBodyKit(bool ItemIsNew)
{
	if (ItemIsNew)
	{
		AmmoInfo.SetNum(5);
		for (int i = 0; i < AmmoInfo.Num(); i++)
		{
			AmmoInfo[i].WeaponType = static_cast<EWeaponType>(i);
			EWeaponType temp = AmmoInfo[i].WeaponType;
			switch (temp)
			{
			case EWeaponType::Pistol:
				AmmoInfo[i].Count = 0;
				AmmoInfo[i].MaxCount = 100;
				break;
			case EWeaponType::ShotGunType:
				AmmoInfo[i].Count = 0;
				AmmoInfo[i].MaxCount = 40;
				break;
			case EWeaponType::Rifle:
				AmmoInfo[i].Count = 0;
				AmmoInfo[i].MaxCount = 200;
				break;
			case EWeaponType::GrenadeLauncher:
				AmmoInfo[i].Count = 0;
				AmmoInfo[i].MaxCount = 10;
				break;
			case EWeaponType::SniperRIfle:
				AmmoInfo[i].Count = 0;
				AmmoInfo[i].MaxCount = 20;
				break;
			default:
				break;
			}
		}
		CheckRarity();
		ERarity ItemRarity = ItemCFG.EquipmentInfo.ItemRarity;

	
		switch (ItemRarity)
		{
		case ERarity::None:
			break;
		case ERarity::Common:

			break;
		case ERarity::Uncommon:
			for (int i = 0; i < AmmoInfo.Num(); i++)
			{
				AmmoInfo[i].MaxCount += 10;
			}
			break;
		case ERarity::Rare:
			for (int i = 0; i < AmmoInfo.Num(); i++)
			{
				AmmoInfo[i].MaxCount += 25;
			}
			break;
		case ERarity::Epic:
			for (int i = 0; i < AmmoInfo.Num(); i++)
			{
				AmmoInfo[i].MaxCount += 40;
			}
			break;
		case ERarity::Legendary:
			for (int i = 0; i < AmmoInfo.Num(); i++)
			{
				AmmoInfo[i].MaxCount += 100;
			}
			break;
		default:
			break;
		}
	
		
	}
	else
	{
		//UProjectXInventoryComponent* myInventory = Cast<UProjectXInventoryComponent>(Character->GetComponentByClass(UProjectXInventoryComponent::StaticClass()));
		//AmmoInfo = myInventory->GetAmmoSlotsInfo();
	}
}
bool APickUpActor::EquipBodyKit()
{
	bool bIsBodyKitEquipSuccess = false;
	UProjectXInventoryComponent* myInventory = Cast<UProjectXInventoryComponent>(Character->GetComponentByClass(UProjectXInventoryComponent::StaticClass()));
	bIsBodyKitEquipSuccess = myInventory->EquipBodyKit(ItemCFG,AmmoInfo);
	return bIsBodyKitEquipSuccess;
}

void APickUpActor::SetBodyKitInfo(TArray<FAmmoSlot> Ammo)
{
	AmmoInfo = Ammo;
	
}

void APickUpActor::InitBracer(bool ItemIsNew, FInventory DropedItem)
{
	if (ItemIsNew)
	{

		int8 Random = FMath::RandRange(1, 3);
		if (Random == 1)
		{
			CurrentBracerSkill = ESkillList::Teleport;
			CoolDown = 30.0f;
		}
		else if (Random == 2)
		{
			CurrentBracerSkill = ESkillList::Recall;
			CoolDown = 180.0f;
		}
		else if (Random == 3)
		{
			CurrentBracerSkill = ESkillList::SlowMode;
			CoolDown = 50.0f;
			AbilityTimer = 5.0f;
		}
		
		CheckRarity();
		ERarity ItemRarity = ItemCFG.EquipmentInfo.ItemRarity;
		switch (ItemRarity)
		{
		case ERarity::None:
			break;
		case ERarity::Common:
		
			break;
		case ERarity::Uncommon:
			CoolDown -= 2.0f;
			AbilityTimer += 1.0f;
			break;
		case ERarity::Rare:
			CoolDown -= 4.0f;
			AbilityTimer += 2.0f;
			break;
		case ERarity::Epic:
			CoolDown -= 8.0f;
			AbilityTimer += 4.0f;
			break;
		case ERarity::Legendary:
			CoolDown -= 16.0f;
			AbilityTimer += 8.0f;
			break;
		default:
			break;
		}

		
	}
	else
	{
		ItemCFG = DropedItem;
	}
}
bool APickUpActor::EquipBracer()
{
	bool bIsBracerEquipSuccess = false;
	UProjectXInventoryComponent* myInventory = Cast<UProjectXInventoryComponent>(Character->GetComponentByClass(UProjectXInventoryComponent::StaticClass()));
	bIsBracerEquipSuccess = myInventory->EquipBracer(ItemCFG,CoolDown, AbilityTimer,CurrentBracerSkill);
	return bIsBracerEquipSuccess;

}
void APickUpActor::InitArmor(bool ItemIsNew)
{
	
	if (ItemIsNew)
	{
		CheckRarity();
		ERarity ItemRarity = ItemCFG.EquipmentInfo.ItemRarity;
		switch (ItemRarity)
		{
		case ERarity::None:
			break;
		case ERarity::Common:
			DefCoef = 0.05f;
			break;
		case ERarity::Uncommon:
			DefCoef = 0.11f;
			break;
		case ERarity::Rare:
			DefCoef = 0.23f;
			break;
		case ERarity::Epic:
			DefCoef = 0.28f;
			break;
		case ERarity::Legendary:
			DefCoef = 0.32f;
			break;
		default:
			break;
		}

		
	}
	else
	{

	}
}
bool APickUpActor::EquipArmor()
{
	bool bIsArmorEquipSuccess = true;
	UProjectXInventoryComponent* myInventory = Cast<UProjectXInventoryComponent>(Character->GetComponentByClass(UProjectXInventoryComponent::StaticClass()));
	myInventory->EquipArmor(ItemCFG, DefCoef); 	
	return bIsArmorEquipSuccess;
}
void APickUpActor::InitWeapon(bool ItemIsNew, FInventory DropedItem)
{
	UProjectXGameInstance* MyGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());
	if (MyGI)
	{
		MyGI->GetWeaponInfoByName(NameOfTheItem, WeaponInfo);
	}
	if (WeaponInfo.WeaponClass)
	{
		if (ItemIsNew)
		{
			CheckRarity();
			ERarity ItemRarity = ItemCFG.EquipmentInfo.ItemRarity;
			switch (ItemRarity)
			{
			case ERarity::None:
				break;
			case ERarity::Common:
				WeaponInfo.ProjectileSetting.ProjectileDamage += 2;
				
				break;
			case ERarity::Uncommon:
				WeaponInfo.ProjectileSetting.ProjectileDamage += 5;
				break;
			case ERarity::Rare:
				WeaponInfo.ProjectileSetting.ProjectileDamage += 9;
				WeaponInfo.RateOfFire -= 0.03;
				WeaponInfo.MaxRound += 4;
				break;
			case ERarity::Epic:
				WeaponInfo.ProjectileSetting.ProjectileDamage += 13;
				WeaponInfo.RateOfFire -= 0.06;
				WeaponInfo.MaxRound += 10;
				break;
			case ERarity::Legendary:
				WeaponInfo.ProjectileSetting.ProjectileDamage += 20;
				WeaponInfo.RateOfFire -= 0.1;
				WeaponInfo.MaxRound += 16;
				break;
			default:
				break;
			}
			
		}
		else
		{
			ItemCFG.EquipmentInfo.ItemRarity = DropedItem.EquipmentInfo.ItemRarity;
		}
	}

	
}
bool APickUpActor::EquipWeapon()
{
	bool bIsWeaponEquipSuccess = false;
	UProjectXInventoryComponent* myInventory = Cast<UProjectXInventoryComponent>(Character->GetComponentByClass(UProjectXInventoryComponent::StaticClass()));
	
	if (myInventory)
	{
		bIsWeaponEquipSuccess = myInventory->EquipWeapon(ItemCFG, WeaponInfo);
	}

	return bIsWeaponEquipSuccess;	
}

void APickUpActor::OverlapStart_BP_Implementation(bool isOverlaping)
{

}
 
