// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "ProjectX/Weapons/ProjectX_StateEffect.h"
#include "Types.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Aim_State UMETA(DisplayName = "Aim State"),
	AimWalk_State UMETA(DisplayName = "AimWalk State"),
	AimCrouch_State UMETA(DisplayName = "AimCrouch State"),
	Walk_State UMETA(DisplayName = "Walk State"),
	Crouch_State UMETA(DisplayName = "Crouch State"),
	Run_State UMETA(DisplayName = "Run State"),
	SprintRun_State UMETA(DisplayName = "SprintRun State")
};
UENUM(BlueprintType)
enum class EConsumableItemsList : uint8
{
	BigHealhPack UMETA(DisplayName = "BigHealhPack"),
	LittleHealthPack UMETA(DisplayName = "LittleHealthPack"),
	Syringe UMETA(DisplayName = "Syringe"),	
	LittleAmmoPack UMETA(DisplayName = "LittleAmmoPack"),
	BigAmmoPack UMETA(DisplayName = "BigAmmoPack"),
};

UENUM(BlueprintType)
enum class ERarity : uint8
{
	None UMETA(DisplayName = "None"),
	Common UMETA(DisplayName = "Common"),
	Uncommon UMETA(DisplayName = "Uncommon"),
	Rare UMETA(DisplayName = "Rare"),
	Epic UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol UMETA(DisplayName = "Pistol"),
	ShotGunType UMETA(DisplayName = "ShotGun"),
	Rifle UMETA(DisplayName = "Rifle"),
	GrenadeLauncher UMETA(DisplayName = "GrenadeLauncher"),
	SniperRIfle UMETA(DisplayName = "SniperRIfle"),
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None UMETA(DisplayName = "None"),
	Consumeable UMETA(DisplayName = "Consumeable"),
	Equipment UMETA(DisplayName = "Equipment"),
	Readables UMETA(DisplayName = "Readables"),
	Ammo UMETA(DisplayName = "Ammo"),
	QuestItem UMETA(DisplayName = "QuestItem")
};

UENUM(BlueprintType)
enum class ESlotType :uint8
{
FirstSlot,
SecondSlot,
ThirdSlot,
FourthSlot
};

UENUM(BlueprintType)
enum class EEquipmentSlotType :uint8
{
	None,
	Weapon,
	Bracer,
	BodyKit,
	Armor,
	BackPack
};

UENUM(BlueprintType)
enum class EStatTypesName :uint8
{
	Strength,
	Constitution,
	Intelligence,
	Reaction
};

USTRUCT(BlueprintType)
struct FStatsInfo
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int32 Strength = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int32 Constitution = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int32 Intelligence = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int32 Reaction = 5;
};



/* 
USTRUCT(BlueprintType)
struct FEquipmenSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EquipmentSlots")
		TSubclassOf<class AMasterItem> MasterItem = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EquipmentSlots")
		FItemsInfo ItemsInfo;

};

USTRUCT(BlueprintType)
struct FInventorySlot 
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventorySlots")
		TSubclassOf<class AMasterItem> MasterItem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomItemInfo")
		FItemsInfo ItemsInfo;
};
*/



USTRUCT(BlueprintType)
struct FCharacterSpeed
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float AimSpeedNormal = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float WalkSpeedNormal = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float RunSpeedNormal = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float AimSpeedWalk = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float SprintRunSpeedRun = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float CrouchSpeedNormal = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float AimCrouchSpeed = 30.0f;
};



USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		TSubclassOf<class AProjectileDefault> Projectile = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float ProjectileDamage = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		UStaticMesh* BulletMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory ")
		UTexture2D* ProjectileIcon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		FTransform BulletMeshTransform = FTransform();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		UParticleSystem* BulletFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		FTransform BulletFXTransform = FTransform();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitEffect ")
		FVector DecalSize = FVector();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitEffect ")
		float DecalLifetime = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float ProjectileLifeTime = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		float ProjectileInitSpeed = 2000.0f;

	//material to decal on hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		TMap<TEnumAsByte<EPhysicalSurface>, UMaterialInterface*> HitDecals;
	//Sound when hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		USoundBase* HitSound = nullptr;
	//fx when hit check by surface
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
		TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> HitFXs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExlosiveProjectileSetting")
		TSubclassOf<UProjectX_StateEffect> Effect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExlosiveProjectileSetting")
		UParticleSystem* ExploseFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExlosiveProjectileSetting")
		USoundBase* ExploseSound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExlosiveProjectileSetting")
		float ProjectileMaxRadiusDamage = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExlosiveProjectileSetting")
		float ExploseMaxDamage = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExlosiveProjectileSetting")
		float MaxRadius = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExlosiveProjectileSetting")
		float MinRadius = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExlosiveProjectileSetting")
		float DamageDecayFromCenter = 5.0f;
	//Timer add
};

USTRUCT(BlueprintType)
struct FWeaponDispersion
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Aim_StateDispersionAimMax = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Aim_StateDispersionAimMin = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Aim_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Aim_StateDispersionReduction = .3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float AimWalk_StateDispersionAimMax = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float AimWalk_StateDispersionAimMin = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float AimWalk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float AimWalk_StateDispersionReduction = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Walk_StateDispersionAimMax = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Walk_StateDispersionAimMin = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Walk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Walk_StateDispersionReduction = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Run_StateDispersionAimMax = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Run_StateDispersionAimMin = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Run_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Run_StateDispersionReduction = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Crouch_StateDispersionMax = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Crouch_StateDispersionMin = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Crouch_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float Crouch_StateDispersionReduction = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float CrouchAim_StateDispersionMax = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float CrouchAim_StateDispersionMin = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float CrouchAim_StateDispersionRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		float CrouchAim_StateDispersionReduction = 0.1f;
};

USTRUCT(BlueprintType)
struct FAnimationWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Char")
		UAnimMontage* AnimCharFire = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Char")
		UAnimMontage* AnimCharFireAim = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Char")
		UAnimMontage* AnimCharReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Char")
		UAnimMontage* AnimCharReloadAim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Weapon")
		UAnimMontage* AnimWeaponReload = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Weapon")
		UAnimMontage* AnimWeaponReloadAim = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Weapon")
		UAnimMontage* AnimWeaponFire = nullptr;
};

USTRUCT(BlueprintType)
struct FMeshInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnMesh")
		UStaticMesh* Mesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh ")
		float DropMeshTime = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh ")
		float DropMeshLifeTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnMesh")
		float Power = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnMesh")
		FVector Scale = FVector();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnMesh")
		FQuat Rotation = FQuat();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh ")
		float ImpulseRandomDispersion = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropMesh ")
		float CustomMass = 0.0f;

};




USTRUCT(BlueprintType)
struct FAddicionalWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")		
	int32 Round = 10;
};

USTRUCT(BlueprintType)
struct FWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
		TSubclassOf<class AWeaponDefault> WeaponClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
		FName WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		float RateOfFire = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		float ReloadTime = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		int32 MaxRound = 10;
	//UPROPERTY(BlueprintReadOnly)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		int32 CurrentRound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		int32 NumberProjectileByShot = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion ")
		FWeaponDispersion DispersionWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound ")
		USoundBase* SoundFireWeapon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound ")
		USoundBase* SoundReloadWeapon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound ")
		USoundBase* SoundEmptyMagFire = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX ")
		UParticleSystem* EffectFireWeapon = nullptr;
	//if null use trace logic (TSubclassOf<class AProjectileDefault> Projectile = nullptr)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile ")
		FProjectileInfo ProjectileSetting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace ")
		float WeaponDamage = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace ")
		float DistacneTrace = 2000.0f;
	//one decal on all?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitEffect ")
		UDecalComponent* DecalOnHit = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh ")
		FMeshInfo Shell;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh ")
		FMeshInfo Magazin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim ")
		FAnimationWeaponInfo AnimWeaponInfo;

	//inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory ")
		float SwitchTimeToWeapon = 1.0f;

	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory ")
		UTexture2D* WeaponIcon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory ")
		EWeaponType WeaponType = EWeaponType::Pistol;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		UStaticMesh* WeaponMesh = nullptr;

};

USTRUCT(BlueprintType)
struct FWeaponSlot
{
	GENERATED_BODY()
		
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSlot")
		FName NameItem = "NoName";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponSlot")
		FAddicionalWeaponInfo AdditionalInfo;
	UPROPERTY()
		FWeaponInfo WeaponInfo;
};

USTRUCT(BlueprintType)
struct FAmmoSlot
{
	GENERATED_BODY()

		///Index Slot by Index Array
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
		EWeaponType WeaponType = EWeaponType::Pistol;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
		int32 Count = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmmoSlot")
		int32 MaxCount = 100;

};

USTRUCT(BlueprintType)
struct FEquipmentInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EquipmentInfo")
		EEquipmentSlotType SlotType = EEquipmentSlotType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EquipmentInfo")
		ERarity ItemRarity = ERarity::None;

};

USTRUCT(BlueprintType)
struct FItemsInfo
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItemInfo")
		FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItemInfo")
		FText ItemDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItemInfo")
		UTexture2D* ItemIcon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItemInfo")
		UStaticMesh* ItemMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItemInfo")
		bool ItemcanBeUsed = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItemInfo")
		FText ItemUseText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItemInfo")
		bool ItemcanBeStacked = true;
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItemInfo")
		int32 AmountOfItemsInSlot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItemInfo")
		int32 MaxItemsStackAtSlot;
	UPROPERTY(BlueprintReadOnly, Category = "InventoryItemInfo")
		bool isSlotOccupied = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItemInfo")
		float ItemWeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItemInfo")
		EItemType Itemtype = EItemType::None;

};

USTRUCT(BlueprintType)
struct FDropItem : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropWeapon")
		UStaticMesh* WeaponStaticMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropWeapon")
		USkeletalMesh* WeaponSkeletalMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropWeapon")
		FWeaponSlot WeaponInfo;

};

USTRUCT(BlueprintType)
struct FInventory : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItem")
		FItemsInfo ItemsInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EquipmentItemInfo")
		bool IsWeapon = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EquipmentItemInfo")
		FEquipmentInfo EquipmentInfo;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	//	int32 AmountOfItems;	

};

UCLASS()
class  PROJECTX_API UTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintCallable)
	static void AddEffectBySurfaceType(AActor* TakeEffectActor,TSubclassOf<UProjectX_StateEffect> AddEffectClass, EPhysicalSurface SurfaceType);
	
};
