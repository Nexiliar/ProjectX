// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectX/FunctionLibrary/Types.h"
#include "ProjectX/Weapons/Projectiles/ProjectileDefault.h"
#include "Animation/AnimInstance.h"
#include "ProjectX/Weapons/ProjectX_StateEffect.h"

#include "WeaponDefault.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFireStart);//ToDo Delegate on event weapon fire - Anim char, state char...
//#define DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStart, UAnimMontage*, Anim);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponReloadEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFireStart, UAnimMontage*, AnimFireChar);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStart, UAnimMontage*, AnimReloadChar);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponReloadEnd, bool, bIsSucces, int32, AmmoSafe);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFireEvent, int32, CurrentAmmo);

UCLASS()
class PROJECTX_API AWeaponDefault : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AWeaponDefault();

	FOnWeaponFireStart OnWeaponFireStart;
	FOnWeaponReloadEnd OnWeaponReloadEnd;
	FOnWeaponReloadStart OnWeaponReloadStart;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "FireEvent")
	FOnFireEvent OnFireEvent;
	FTimerHandle TimerHandle_EmptyMagShootTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* StaticMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* ShootLocation = nullptr;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* MagazineDropLocation = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* AmmoClipDropLocation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
		FWeaponInfo WeaponSetting;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	//	FAddicionalWeaponInfo WeaponInfo;

	FTimerHandle TimerHandle_BurstFire;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void FireTick(float DeltaTime);
	void ReloadTick(float DeltaTime);
	void DispersionTick(float DeltaTime);
	void ClipDropTick(float DeltaTime);
	void ShellDropTick(float DeltaTime);

	void WeaponInit();

	

	UPROPERTY()
		bool WeaponFiring = false;
	UPROPERTY()
		bool WeaponReloading = false;
		bool WeaponAiming = false;
	UFUNCTION(BlueprintCallable)
		void SetWeaponStateFire(bool bIsFire);

		bool CheckWeaponCanFire();

	FProjectileInfo GetProjectile();
	UFUNCTION()
		void Fire();
	UFUNCTION()
		void BurstFire();
	UPROPERTY()
		int8 TempVarForBurst = 0;
	UPROPERTY()
		bool isBurst = false;
		
	UFUNCTION()
		void DropBulletEmptyShell(UStaticMesh* shell, FVector scale, FQuat rotation, float impulsepower);
	//shell flag
		bool DropShellFlag = false;
		float DropShellTimer = -1.0f;
	UFUNCTION()
		void DropEmptyMagazin(UStaticMesh* magazin, FVector scale, FQuat rotation);
	//Timer Drop Magazine on reload
		bool DropClipFlag = false;
		float DropClipTimer = -1.0;

	void UpdateStateWeapon(EMovementState NewMovementState, float StatsAdjust);
	void ChangeDispersionByShot();
	float GetCurrentDispersion() const;
	
	int8 GetNumberProjectileByShot() const;

	//Timers'flags
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		float FireTimer = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
		float ReloadTimer = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic Debug")	//Remove !!! Debug
		float ReloadTime = 0.0f;
	
	//flags
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckFireLogic")
		bool BlockFire = false;

	UPROPERTY()
		float FireRateStatAdjust = 0.0f;
		
	
	//Dispersion
	bool ShouldReduceDispersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 0.1f;
	float CurrentDispersionReduction = 0.1f;

	FVector ApplyDispersionToShoot(FVector DirectionShoot) const;
	
	FVector ShootEndLocation = FVector(0);
	UFUNCTION(BlueprintCallable)
	FVector GetFireEndLocation() const;

	UFUNCTION(BlueprintCallable)
		int32 GetWeaponRound();
	UFUNCTION()
	void InitReload();
	void FinishReload();
	void CancelReload();

	void EmptyMagTryToShoot();
	void ClickTimer();
	bool CheckCanWeaponReload();
	int32 GetAviableAmmoForReload();
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool ShowDebug = false;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		float SizeVectorToChangeShootDirectionLogic = 100.0f;

};

