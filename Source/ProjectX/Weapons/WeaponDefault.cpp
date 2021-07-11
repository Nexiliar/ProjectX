// Fill out your copyright notice in the Description page of Project Settings.
#include "WeaponDefault.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectX/Character/ProjectXInventoryComponent.h"

// Sets default values
AWeaponDefault::AWeaponDefault()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh "));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);

	MagazineDropLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("MagazineDropLocation"));
	MagazineDropLocation->SetupAttachment(RootComponent);
	
	
	AmmoClipDropLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("AmmoClipDropLocation"));
	AmmoClipDropLocation->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AWeaponDefault::BeginPlay()
{
	Super::BeginPlay();
	WeaponSetting.CurrentRound = WeaponSetting.MaxRound;
	WeaponInit();
	
}

// Called every frame
void AWeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	DispersionTick(DeltaTime);
	ClipDropTick(DeltaTime);
	ShellDropTick(DeltaTime);
}

void AWeaponDefault::FireTick(float DeltaTime)
{
	if(FireTimer > 0.0f)
	FireTimer -= DeltaTime;
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, TEXT("GetWeaponRound()"));
	if (WeaponFiring && GetWeaponRound() > 0 && !WeaponReloading && FireTimer < 0.0f)
	{
					Fire();			
	}
	
}

void AWeaponDefault::ReloadTick(float DeltaTime)
{
	if (WeaponReloading)
	{
		if (ReloadTimer < 0.0f)
		{
			FinishReload();
		}
		else
		{
			ReloadTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::DispersionTick(float DeltaTime)
{
	if (!WeaponReloading)
	{
		if (!WeaponFiring)
		{
			if (ShouldReduceDispersion)
				CurrentDispersion = CurrentDispersion - CurrentDispersionReduction;
			else
				CurrentDispersion = CurrentDispersion + CurrentDispersionReduction;
		}

		if (CurrentDispersion < CurrentDispersionMin)
		{

			CurrentDispersion = CurrentDispersionMin;

		}
		else
		{
			if (CurrentDispersion > CurrentDispersionMax)
			{
				CurrentDispersion = CurrentDispersionMax;
			}
		}
	}
	if (ShowDebug)
		UE_LOG(LogTemp, Warning, TEXT("Dispersion: MAX = %f. MIN = %f. Current = %f"), CurrentDispersionMax, CurrentDispersionMin, CurrentDispersion);
}

void AWeaponDefault::ClipDropTick(float DeltaTime)
{
	if (DropClipFlag)
	{
		if (DropClipTimer < 0.0f)
		{
			DropClipFlag = false;
			DropEmptyMagazin(WeaponSetting.Magazin.Mesh, WeaponSetting.Magazin.Scale, WeaponSetting.Magazin.Rotation);
		}
		else
			DropClipTimer -= DeltaTime;
	}
}

void AWeaponDefault::ShellDropTick(float DeltaTime)
{
	if (DropShellFlag)
	{
		if (DropShellTimer < 0.0f)
		{
			DropShellFlag = false;
			DropBulletEmptyShell(WeaponSetting.Shell.Mesh, WeaponSetting.Shell.Scale, WeaponSetting.Shell.Rotation, WeaponSetting.Shell.Power);
		}
		else
			DropShellTimer -= DeltaTime;
	}
}




void AWeaponDefault::WeaponInit()
{
	FireTimer = WeaponSetting.RateOfFire;
	
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}

	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent();
	}
	
	UpdateStateWeapon(EMovementState::Run_State);
	
}

void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	if (CheckWeaponCanFire())
		WeaponFiring = bIsFire;
	else
		WeaponFiring = false;
	//FireTimer = 0.01f;//!!!!!
}

bool AWeaponDefault::CheckWeaponCanFire()
{
	return !BlockFire;
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
	return WeaponSetting.ProjectileSetting;
}

void AWeaponDefault::Fire()
{
	UAnimMontage* AnimToPlay = nullptr;
	if (WeaponAiming)
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharFireAim;
	else
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharFire;
	if (WeaponSetting.AnimWeaponInfo.AnimWeaponFire && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(WeaponSetting.AnimWeaponInfo.AnimWeaponFire);
		
	}
	if (WeaponSetting.Shell.Mesh)
	{
		if (WeaponSetting.Shell.DropMeshTime< 0.0f)
		{
			DropBulletEmptyShell(WeaponSetting.Shell.Mesh, WeaponSetting.Shell.Scale, WeaponSetting.Shell.Rotation, WeaponSetting.Shell.Power);
		}
		else
		{
			DropShellFlag = true;
			DropShellTimer = WeaponSetting.Shell.DropMeshTime;
		}
	}
	

	int8 NumberProjectile = GetNumberProjectileByShot();
	
	FireTimer = WeaponSetting.RateOfFire;
	WeaponSetting.CurrentRound = WeaponSetting.CurrentRound - NumberProjectile;
	
	if (WeaponSetting.CurrentRound == 0)
	{
		EmptyMagTryToShoot();
	}

	ChangeDispersionByShot();

	OnWeaponFireStart.Broadcast(AnimToPlay);
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSetting.SoundFireWeapon, ShootLocation->GetComponentLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSetting.EffectFireWeapon, ShootLocation->GetComponentTransform());
	

	
	if (ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();
		

		for (int8 i = 0; i < NumberProjectile; i++)//ShotGunLogic spawn more then 1 bullet
		{
			FVector EndLocation = GetFireEndLocation();
			//¬ычисл€ем вектор между спавном и конечной точкой выстрела, нормализуем его
			FVector Dir = EndLocation - SpawnLocation;
			Dir.Normalize();
			//—оздаем матрицу, из которой получаем rotation дл€ спавна пули
			FMatrix myMatrix(Dir, FVector(0, 1, 0), FVector(0, 0, 1), FVector::ZeroVector);
			FRotator SpawnRotation = myMatrix.Rotator();

			if (ProjectileInfo.Projectile)
			{
				//Projectile Init ballistic fire

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner =this;
				SpawnParams.Instigator = GetInstigator();

				AProjectileDefault* myProjectile = Cast<AProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myProjectile)
				{
					myProjectile->InitProjectile(WeaponSetting.ProjectileSetting);
				}
			}
			else
			{
				//ToDo Projectile null Init trace fire			
				FHitResult Hit;
				FVector Forward = ShootLocation->GetForwardVector() * 1000+ SpawnLocation;
				
				GetWorld()->LineTraceSingleByChannel(Hit, SpawnLocation, Forward, ECollisionChannel::ECC_Visibility,FCollisionQueryParams::DefaultQueryParam);
				DrawDebugLine(GetWorld(), SpawnLocation, Forward, FColor::Blue, false, 5.f, (uint8)'\000', 0.8f);
												
						EPhysicalSurface mySurfacetype = UGameplayStatics::GetSurfaceType(Hit);

						if (WeaponSetting.ProjectileSetting.HitDecals.Contains(mySurfacetype))
						{
							UMaterialInterface* myMaterial = WeaponSetting.ProjectileSetting.HitDecals[mySurfacetype];

							if (myMaterial && Hit.GetComponent())
							{
								UGameplayStatics::SpawnDecalAttached(myMaterial, FVector(0.5f), Hit.GetComponent(), NAME_None, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
							}
						}
						if (WeaponSetting.ProjectileSetting.HitFXs.Contains(mySurfacetype))
						{
							UParticleSystem* myParticle = WeaponSetting.ProjectileSetting.HitFXs[mySurfacetype];
							if (myParticle)
							{
								UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myParticle, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(1.0f)));
							}
						}

						if (WeaponSetting.ProjectileSetting.HitSound)
						{
							UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSetting.ProjectileSetting.HitSound, Hit.ImpactPoint);
						}


							UTypes::AddEffectBySurfaceType(Hit.GetActor(), ProjectileInfo.Effect, mySurfacetype);

						
						/* ≈сли вызов происходит из BP
						if (Hit.GetActor()->GetClass()->ImplementsInterface(UProjectX_Interface_GameActor::StaticClass()))
						{
							IProjectX_Interface_GameActor::Execute_AvailableForEffects(Hit.GetActor());

							IProjectX_Interface_GameActor::Execute_AvailableForEffectsBP(Hit.GetActor());
						}
						*/
					//	UProjectX_StateEffect* NewEffect = NewObject<UProjectX_StateEffect>(Hit.GetActor(), FName("Effect"));

					UGameplayStatics::ApplyDamage(Hit.GetActor(), WeaponSetting.ProjectileSetting.ProjectileDamage, GetInstigatorController(), this, NULL);
				
			}
		}
		
		
	}
//	if(inventoryComponent)
}




void AWeaponDefault::DropBulletEmptyShell(UStaticMesh* shell, FVector scale, FQuat rotation, float impulsepower)
{
	FTransform SpawnTransform = AmmoClipDropLocation->GetComponentTransform();
	AStaticMeshActor* BulletShell = nullptr;
	SpawnTransform.SetRotation(rotation);
	SpawnTransform.SetScale3D(scale);
	FVector ImpulseLocation = AmmoClipDropLocation->GetForwardVector()* AmmoClipDropLocation->GetRightVector();
	BulletShell = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SpawnTransform);
	//SpawnedMeshParams
	BulletShell->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
	BulletShell->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	BulletShell->GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
	BulletShell->GetStaticMeshComponent()->SetSimulatePhysics(true);
	BulletShell->GetStaticMeshComponent()->SetStaticMesh(shell);
	BulletShell->SetActorTickEnabled(false);
	BulletShell->InitialLifeSpan = WeaponSetting.Shell.DropMeshLifeTime;
	BulletShell->GetStaticMeshComponent()->AddImpulse(ImpulseLocation * impulsepower * BulletShell->GetStaticMeshComponent()->GetMass());
}

void AWeaponDefault::DropEmptyMagazin(UStaticMesh* magazin, FVector scale, FQuat rotation)
{
	FTransform SpawnTransform = MagazineDropLocation->GetComponentTransform();
	AStaticMeshActor* Magazin = nullptr;
	SpawnTransform.SetRotation(rotation);
	SpawnTransform.SetScale3D(scale);
	Magazin = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SpawnTransform);
	Magazin->GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
	Magazin->GetStaticMeshComponent()->SetSimulatePhysics(true);
	Magazin->SetActorTickEnabled(false);
	Magazin->InitialLifeSpan = WeaponSetting.Shell.DropMeshLifeTime;
	Magazin->GetStaticMeshComponent()->SetStaticMesh(magazin);
	
}

void AWeaponDefault::UpdateStateWeapon(EMovementState NewMovementState)
{
	//ToDo Dispersion
	BlockFire = false;
	switch (NewMovementState)
	{
	case EMovementState::Aim_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::AimWalk_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::AimCrouch_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.CrouchAim_StateDispersionMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.CrouchAim_StateDispersionMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.CrouchAim_StateDispersionRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.CrouchAim_StateDispersionReduction;
		break;
	case EMovementState::Walk_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::Crouch_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Crouch_StateDispersionMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Crouch_StateDispersionMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Crouch_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Crouch_StateDispersionReduction;
		break;
	case EMovementState::Run_State:
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Run_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Run_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Run_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::SprintRun_State:
		BlockFire = true;
		SetWeaponStateFire(false);//set fire trigger to false
		//Block Fire
		break;
	default:
		break;
	}


}

void AWeaponDefault::ChangeDispersionByShot()
{
	CurrentDispersion = CurrentDispersion + CurrentDispersionRecoil;
}

float AWeaponDefault::GetCurrentDispersion() const
{
	float Result = CurrentDispersion;
	return Result;
}





int8 AWeaponDefault::GetNumberProjectileByShot() const
{
	return WeaponSetting.NumberProjectileByShot;
}

FVector AWeaponDefault::ApplyDispersionToShoot(FVector DirectionShoot) const
{
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI / 180.f);
}

FVector AWeaponDefault::GetFireEndLocation() const
{
	bool bShootDirection = false;
	FVector EndLocation = FVector(0.f);

	FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);
	//UE_LOG(LogTemp, Warning, TEXT("Vector: X = %f. Y = %f. Size = %f"), tmpV.X, tmpV.Y, tmpV.Size());

	if (tmpV.Size() > SizeVectorToChangeShootDirectionLogic)
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot((ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;
		if (ShowDebug)
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), -(ShootLocation->GetComponentLocation() - ShootEndLocation), WeaponSetting.DistacneTrace, GetCurrentDispersion() * PI / 180.f, GetCurrentDispersion() * PI / 180.f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);
	}
	else
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;
		if (ShowDebug)
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetForwardVector(), WeaponSetting.DistacneTrace, GetCurrentDispersion() * PI / 180.f, GetCurrentDispersion() * PI / 180.f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);
	}


	if (ShowDebug)
	{
		//direction weapon look
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector() * 500.0f, FColor::Cyan, false, 5.f, (uint8)'\000', 0.5f);
		//direction projectile must fly
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootEndLocation, FColor::Red, false, 5.f, (uint8)'\000', 0.5f);
		//Direction Projectile Current fly
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), EndLocation, FColor::Green, false, 5.f, (uint8)'\000', 0.5f);

		//DrawDebugSphere(GetWorld(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector()*SizeVectorToChangeShootDirectionLogic, 10.f, 8, FColor::Red, false, 4.0f);
	}


	return EndLocation;
}

int32 AWeaponDefault::GetWeaponRound()
{
	return WeaponSetting.CurrentRound;
	
}

void AWeaponDefault::InitReload()
{
	
		
	WeaponReloading = true;
	ReloadTimer = WeaponSetting.ReloadTime;
	UAnimMontage* AnimToPlay = nullptr;
	if (WeaponAiming)
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharReloadAim;
	else
		AnimToPlay = WeaponSetting.AnimWeaponInfo.AnimCharReload;
	
	OnWeaponReloadStart.Broadcast(AnimToPlay);
	
	UAnimMontage* AnimWeaponToPlay = nullptr;
	if (WeaponAiming)
		AnimWeaponToPlay = WeaponSetting.AnimWeaponInfo.AnimWeaponReloadAim;
	else
		AnimWeaponToPlay = WeaponSetting.AnimWeaponInfo.AnimWeaponReload;

	if (WeaponSetting.AnimWeaponInfo.AnimWeaponReload
		&& SkeletalMeshWeapon
		&& SkeletalMeshWeapon->GetAnimInstance())//Bad Code? maybe best way init local variable or in func
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(AnimWeaponToPlay);
	}
	if(WeaponSetting.Magazin.Mesh)
		DropClipFlag = true;
		DropEmptyMagazin(WeaponSetting.Magazin.Mesh, WeaponSetting.Magazin.Scale, WeaponSetting.Magazin.Rotation);
}

void AWeaponDefault::FinishReload()
{
	WeaponReloading = false;
	int32 AvailableAmmoFromInventory = GetAviableAmmoForReload();
	int32 AmmoNeedToTakeFromInv;
	int32 NeedToReload = WeaponSetting.MaxRound - WeaponSetting.CurrentRound;

	if (NeedToReload > AvailableAmmoFromInventory)
	{
		WeaponSetting.CurrentRound = AvailableAmmoFromInventory;
		AmmoNeedToTakeFromInv = AvailableAmmoFromInventory;
	}
	else
	{
		WeaponSetting.CurrentRound += NeedToReload;
		AmmoNeedToTakeFromInv = NeedToReload;
	}

	OnWeaponReloadEnd.Broadcast(true, -AmmoNeedToTakeFromInv);
}

void AWeaponDefault::CancelReload()
{
	WeaponReloading = false;
	if (SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
		SkeletalMeshWeapon->GetAnimInstance()->StopAllMontages(0.15f);

	OnWeaponReloadEnd.Broadcast(false,0);
	DropClipFlag = false;
}

void AWeaponDefault::EmptyMagTryToShoot()
{
	if (GetWeaponRound() == 0 && WeaponFiring && !WeaponReloading && WeaponSetting.SoundEmptyMagFire && GetWorld())
	{	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EmptyMagShootTimer, this, &AWeaponDefault::ClickTimer, WeaponSetting.RateOfFire, true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_EmptyMagShootTimer);
	}
}

void AWeaponDefault::ClickTimer()
{	
		UE_LOG(LogTemp, Warning, TEXT("FireEmptyMag"));
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSetting.SoundEmptyMagFire, ShootLocation->GetComponentLocation());
}

bool AWeaponDefault::CheckCanWeaponReload()
{
	bool result = true;

	if (GetOwner())
	{
		UProjectXInventoryComponent* myInventory = Cast<UProjectXInventoryComponent>(GetOwner()->GetComponentByClass(UProjectXInventoryComponent::StaticClass()));
		if (myInventory)
		{
			int32 AvailableAmmoForWeapon = WeaponSetting.MaxRound;
			if (!myInventory->CheckAmmoForWeapon(WeaponSetting.WeaponType, AvailableAmmoForWeapon))
			{

				result = false;
			}
		}

	}
	return result;
}

int32 AWeaponDefault::GetAviableAmmoForReload()
{

	int32 AvailableAmmoForWeapon = WeaponSetting.MaxRound;

	if (GetOwner())
	{
		UProjectXInventoryComponent* myInventory = Cast<UProjectXInventoryComponent>(GetOwner()->GetComponentByClass(UProjectXInventoryComponent::StaticClass()));
		if (myInventory)
		{
			
			if (myInventory->CheckAmmoForWeapon(WeaponSetting.WeaponType, AvailableAmmoForWeapon))
			{
				AvailableAmmoForWeapon = AvailableAmmoForWeapon;
			}
		}

	}
	return AvailableAmmoForWeapon;
}



