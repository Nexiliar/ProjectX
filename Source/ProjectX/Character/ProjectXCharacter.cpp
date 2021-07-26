// Copyright Epic Games, Inc. All Rights Reserved.

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT(text), fstring))
#define printvector(variable)  if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, FString::Printf(TEXT(#variable ": %s"), *variable.ToCompactString()), false)

#include "ProjectXCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/World.h"
#include "ProjectX/Game/ProjectXGameInstance.h"

AProjectXCharacter::AProjectXCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InventoryComponent = CreateDefaultSubobject<UProjectXInventoryComponent>(TEXT("InventoryComponent"));
	DropSpawnSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DropWeaponPoint"));
	
	CharHealthComponent = CreateDefaultSubobject<UProjectXCharacterHealthComponent>(TEXT("HealthComponent"));

	SkillComponent = CreateDefaultSubobject<UProjectXSkillComponent>(TEXT("SkillComponent"));
	
	StatsComponent = CreateDefaultSubobject<UProjectXStatsComponent>(TEXT("StatsComponent"));
	if (CharHealthComponent)
	{
		CharHealthComponent->OnDead.AddDynamic(this, &AProjectXCharacter::CharDead);
	}


	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AProjectXCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}	
	GetWorldTimerManager().SetTimer(DelayTimer, this, &AProjectXCharacter::Compare, 0.1f, true, 0.1f);

}
void AProjectXCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CurrentCursor)
	{
		APlayerController* myPC = Cast<APlayerController>(GetController());
		if (myPC)
		{
			FHitResult TraceHitResult;
			myPC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();

			CurrentCursor->SetWorldLocation(TraceHitResult.Location);
			CurrentCursor->SetWorldRotation(CursorR);
		}
	}
	MovementTick(DeltaSeconds);	
}

void AProjectXCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
	Super::SetupPlayerInputComponent(NewInputComponent);
	NewInputComponent->BindAxis(TEXT("MoveForward"), this, &AProjectXCharacter::InputAxisX);
	NewInputComponent->BindAxis(TEXT("MoveRight"), this, &AProjectXCharacter::InputAxisY);
	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::InputAttackPressed);
	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Released, this, &AProjectXCharacter::InputAttackReleased);
	NewInputComponent->BindAction(TEXT("Equip"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::EquipWeapon);
	NewInputComponent->BindAction(TEXT("ReloadEvent"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::TryReloadWeapon);
	
	NewInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::TryToInterractWithObject);
	NewInputComponent->BindAction(TEXT("DropDaBomb"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::DropTheBomb);
	//Old switch sys del if not use
	NewInputComponent->BindAction(TEXT("SwitchNextSkill"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::SwitchSkills);
	//NewInputComponent->BindAction(TEXT("SwitchPrevioseSkill"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::TrySwitchPreviosWeapon);
	NewInputComponent->BindAction(TEXT("AbillityAction"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::TryUseAbillity);
	
	
	NewInputComponent->BindAction(TEXT("WeaponSlot1"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::SwitchWeaponToSlotNum<ESlotType::FirstSlot>);
	NewInputComponent->BindAction(TEXT("WeaponSlot2"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::SwitchWeaponToSlotNum<ESlotType::SecondSlot>);
	NewInputComponent->BindAction(TEXT("WeaponSlot3"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::SwitchWeaponToSlotNum<ESlotType::ThirdSlot>);
	NewInputComponent->BindAction(TEXT("WeaponSlot4"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::SwitchWeaponToSlotNum<ESlotType::FourthSlot>);
}

void AProjectXCharacter::InputAxisX(float Value)
{
	AxisX = Value;
}



void AProjectXCharacter::DropTheBomb()
{
	if (SkillComponent->isRageModeOn && !IsBombDropped)
	{
		FVector SpawnLocation = GetMesh()->GetSocketLocation("SpawnItemSocket");;
		FActorSpawnParameters SpawnParams;
		FRotator SpawnRotation(1, 1, 1);
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		IsBombDropped = true;
		AMyProjectileDefault_Grenade1* Bomb = Cast<AMyProjectileDefault_Grenade1>(GetWorld()->SpawnActor(BombNade, &SpawnLocation, &SpawnRotation, SpawnParams));
			if (Bomb)
			{
				UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon -NULL"));
			}
	}
}

void AProjectXCharacter::SwitchSkills()
{
	SkillComponent->SwitchSkills();
}

void AProjectXCharacter::InputAxisY(float Value)
{
	AxisY = Value;
}

void AProjectXCharacter::InputAttackPressed()
{
	if (bIsAlive)
	{
		AttackCharEvent(true);
	}	
}

void AProjectXCharacter::InputAttackReleased()
{
	AttackCharEvent(false);
}

void AProjectXCharacter::MovementTick(float Deltatime)
{
	if (bIsAlive)
	{
		AddMovementInput(FVector(1.0f, 0.0f, 0.0), AxisX);
		AddMovementInput(FVector(0.0f, 1.0f, 0.0), AxisY);

		if (MovementState == EMovementState::SprintRun_State)
		{
			FVector myRotationVector = FVector(AxisX, AxisY, 0.0f);
			FRotator myRotator = myRotationVector.ToOrientationRotator();
			SetActorRotation((FQuat(myRotator)));
		}
		else
		{
			APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (myController)
			{
				FHitResult ResultHit;
				//myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);// bug was here Config\DefaultEngine.Ini
				myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);

				float FindRotaterResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
				SetActorRotation(FQuat(FRotator(0.0f, FindRotaterResultYaw, 0.0f)));

				if (CurrentWeapon)
				{
					FVector Displacement = FVector(0);
					switch (MovementState)
					{
					case EMovementState::Aim_State:
						Displacement = FVector(0.0f, 0.0f, 160.0f);
						CurrentWeapon->ShouldReduceDispersion = true;
						break;
					case EMovementState::AimWalk_State:
						CurrentWeapon->ShouldReduceDispersion = true;
						Displacement = FVector(0.0f, 0.0f, 160.0f);
						break;
					case EMovementState::AimCrouch_State:
						CurrentWeapon->ShouldReduceDispersion = true;
						Displacement = FVector(0.0f, 0.0f, 80.0f);
						break;
					case EMovementState::Walk_State:
						Displacement = FVector(0.0f, 0.0f, 120.0f);
						CurrentWeapon->ShouldReduceDispersion = false;
						break;
					case EMovementState::Crouch_State:
						CurrentWeapon->ShouldReduceDispersion = true;
						Displacement = FVector(0.0f, 0.0f, 80.0f);
						break;
					case EMovementState::Run_State:
						Displacement = FVector(0.0f, 0.0f, 120.0f);
						CurrentWeapon->ShouldReduceDispersion = false;
						break;
					case EMovementState::SprintRun_State:
						break;
					default:
						break;
					}
					//Определяем место в которое полетят пули
					CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
					//aim cursor like 3d Widget?
				}
			}
		}
	}
}


void AProjectXCharacter::AttackCharEvent(bool bIsFiring)
{
		AWeaponDefault* myWeapon = nullptr;
		myWeapon = GetCurrentWeapon();
		if (myWeapon)
		{			
			//ToDo Check melee or range
			myWeapon->SetWeaponStateFire(bIsFiring);
			myWeapon->EmptyMagTryToShoot();
		}
		//else
			//UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon -NULL"));
}

void AProjectXCharacter::CharacterUpdate()
{
	
	switch (MovementState)
	{
	case EMovementState::Aim_State:
		if(!isOverloaded)
		ResSpeed = MovementSpeedInfo.AimSpeedNormal;
		break;
	case EMovementState::AimWalk_State:
		if (!isOverloaded)
		ResSpeed = MovementSpeedInfo.AimSpeedWalk;
		break;
	case EMovementState::AimCrouch_State:
		if (!isOverloaded)
		ResSpeed = MovementSpeedInfo.AimCrouchSpeed;
		break;
	case EMovementState::Walk_State:
		if (!isOverloaded)
		ResSpeed = MovementSpeedInfo.WalkSpeedNormal;
		break;
	case EMovementState::Crouch_State:
		if (!isOverloaded)
		ResSpeed = MovementSpeedInfo.CrouchSpeedNormal;
		break;
	case EMovementState::Run_State:
		if (!isOverloaded)
		ResSpeed = MovementSpeedInfo.RunSpeedNormal;
		break;
	case EMovementState::SprintRun_State:
		if (!isOverloaded)
		ResSpeed = MovementSpeedInfo.SprintRunSpeedRun;
		break;
	case EMovementState::Overloaded_State:
		
		ResSpeed = MovementSpeedInfo.Overloaded;
		break;
	case EMovementState::CantMoove_State:
		ResSpeed = MovementSpeedInfo.CantMove;
	default:
		break;
	}
	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void AProjectXCharacter::ChangeMovementState()
{	
	FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
	if (!WalkEnabled && !SprintRunEnabled && !AimEnabled && !CrouchEnabled && !isOverloaded)
	{
		MovementState = EMovementState::Run_State;
		if (CurrentWeapon)
			if (IsRifle)
			{
				CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketNoAim"));
			}
			else
			{
				CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
			}
	}
	else
	{
		if (CrouchEnabled && !WalkEnabled && !SprintRunEnabled && !AimEnabled && !isOverloaded)
		{
			MovementState = EMovementState::Crouch_State;
			if (CurrentWeapon)
				if (IsRifle)
				{
					CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketCrouch"));
				}
				else
				{
					CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketPistolCrouch"));
				}				
		}
		if (AimEnabled && CrouchEnabled && !WalkEnabled && !SprintRunEnabled && !isOverloaded)
		{
			MovementState = EMovementState::AimCrouch_State;
			if (CurrentWeapon)
				if (IsRifle)
				{
					CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketCrouch"));
				}
				else
				{
					CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketPistolCrouch"));
				}
		}
		if (SprintRunEnabled && !isOverloaded)
		{
			WalkEnabled = false;
			AimEnabled = false;
			MovementState = EMovementState::SprintRun_State;
			
		}
		if (WalkEnabled && !SprintRunEnabled && AimEnabled && !CrouchEnabled && !isOverloaded)
		{
			MovementState = EMovementState::AimWalk_State;
			if (CurrentWeapon)
			CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
		}
		else
		{
			if (WalkEnabled && !SprintRunEnabled && !AimEnabled && !CrouchEnabled && !isOverloaded)
			{
				MovementState = EMovementState::Walk_State;
				if (CurrentWeapon)
				CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketNoAim"));
			}
			else
			{
				if (!WalkEnabled && !SprintRunEnabled && AimEnabled && !CrouchEnabled && !isOverloaded)
				{
					MovementState = EMovementState::Aim_State;
					if (CurrentWeapon)
					CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
				}
			}
		}
		if (isOverloaded)
		{
			MovementState = EMovementState::Overloaded_State;
		}
	}
	CharacterUpdate();
	//Weapon state update
	AWeaponDefault* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{		
		myWeapon->UpdateStateWeapon(MovementState, CorrectAccuracyOnStatUp);
	}
}

AWeaponDefault* AProjectXCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}
//Сравниваем вектор движения и направление камеры. Если они равны, разрешать спринт
void AProjectXCharacter::Compare()
{
	FVector Velocity = AActor::GetVelocity();
	FVector Forward = AActor::GetActorForwardVector();
	FVector NVelocity = Velocity.GetSafeNormal2D(0.0001f);
		
	
	bool CompareEqual = Forward.Equals(NVelocity, 0.5f);;
		
	if (CompareEqual && SprintCheck)
	{		
		SprintRunEnabled = true;
		
		ChangeMovementState();		
	}
	else
	{		
		SprintRunEnabled = false;
		ChangeMovementState();
	}
	
}

UDecalComponent* AProjectXCharacter::GetCursorToWorld()
{
	return CurrentCursor;
}

void AProjectXCharacter::InitWeapon(FWeaponInfo InfoOfWeaponToInit)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	if (InfoOfWeaponToInit.WeaponClass)
	{
			FVector SpawnLocation = FVector(0);
			FRotator SpawnRotation = FRotator(0);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(InfoOfWeaponToInit.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
			if (myWeapon)
			{					
				FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
				myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));								
								
				myWeapon->WeaponSetting = InfoOfWeaponToInit;
				
				//StatsSysManage
				myWeapon->UpdateStateWeapon(MovementState, CorrectAccuracyOnStatUp);
				myWeapon->FireRateStatAdjust = CorrectFireRateOnStatUp;
					
				//myWeapon->WeaponInfo = WeaponAdditionalInfo;
				//if (InventoryComponent)
				//CurrentIndexWeapon = InventoryComponent->GetWeaponIndexSlotByName(IdWeaponName);
				//Слушаем делегатов, объявленных в weapondefault
				CurrentWeapon = myWeapon;
				myWeapon->OnWeaponReloadStart.AddDynamic(this, &AProjectXCharacter::WeaponReloadStart);
				myWeapon->OnWeaponReloadEnd.AddDynamic(this, &AProjectXCharacter::WeaponReloadEnd);
				myWeapon->OnWeaponFireStart.AddDynamic(this, &AProjectXCharacter::WeaponFireStart);
				
				//Пытаемся перезарядиться, после подбора оружия
				if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
					CurrentWeapon->InitReload();	
				
				if (InventoryComponent && CurrentWeapon)
					InventoryComponent->OnWeaponInit.Broadcast(CurrentWeapon->WeaponSetting.CurrentRound, CurrentWeapon->WeaponSetting);
				
				//Fof equip anim
				if (CurrentWeapon && CurrentWeapon->WeaponSetting.WeaponType == EWeaponType::Pistol)
				{
					IsRifle = false;
				}
				else
				{
					IsRifle = true;
				}
			}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::InitWeapon - CurrentWeapon -NULL"));
	}
}

void AProjectXCharacter::TryReloadWeapon()
{		
	if (bIsAlive && CurrentWeapon && !CurrentWeapon->WeaponReloading)
	{
		if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound)
		{
			if (CurrentWeapon->CheckCanWeaponReload())
			{
				CurrentWeapon->InitReload();
			}
			else
			{
				OnFailedToReload.Broadcast();
			}
		}			
	}
}

void AProjectXCharacter::WeaponReloadStart(UAnimMontage* Anim)
{
	WeaponReloadStart_BP(Anim);
}

void AProjectXCharacter::WeaponReloadEnd(bool bIsSucces,int32 AmmoTake)
{
	
	if (InventoryComponent)
	{
		int32 temp;
		InventoryComponent->SetAmmoInBodyKit(CurrentWeapon->WeaponSetting.WeaponType, AmmoTake, temp);
		InventoryComponent->SetCurrentAmmo(EnumIndex, CurrentWeapon->WeaponSetting.CurrentRound);
	}
	WeaponReloadEnd_BP(bIsSucces);
}

void AProjectXCharacter::WeaponFireStart(UAnimMontage* Anim)
{
	if (InventoryComponent && CurrentWeapon)
		InventoryComponent->SetCurrentAmmo(EnumIndex, CurrentWeapon->WeaponSetting.CurrentRound);
	WeaponFireStart_BP(Anim);
}

void AProjectXCharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
}

void AProjectXCharacter::WeaponReloadEnd_BP_Implementation(bool bIsSucces)
{
}

void AProjectXCharacter::WeaponFireStart_BP_Implementation(UAnimMontage* Anim)
{
}

void AProjectXCharacter::EquipWeapon()
{
	if (InventoryComponent && IsArmed)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->Destroy();
			CurrentWeapon = nullptr;
		}
		IsArmed = false;
	}
	else
	{
		
		int32 TempIndexSlot;
		ESlotType WeaponSlot = GetCurrentSlot(TempIndexSlot);
			SwitchWeaponToSlotNum(WeaponSlot);
		IsArmed = true;
	}
}

ESlotType AProjectXCharacter::SwitchWeaponToSlotNum(ESlotType EquipmentSlotNumber)
{	
	FWeaponInfo WeaponInfoAtIndex;
		switch (EquipmentSlotNumber)
		{
			
			case ESlotType::FirstSlot:
			{
				WeaponInfoAtIndex = InventoryComponent->GetWeaponInfo(0);
					InitWeapon(WeaponInfoAtIndex);
					EnumIndex = 0;
					SlotVar = ESlotType::FirstSlot;
					if (!IsArmed)
						IsArmed = true;
					
				break;
			}
			case ESlotType::SecondSlot:
			{
					WeaponInfoAtIndex = InventoryComponent->GetWeaponInfo(1);		

						InitWeapon(WeaponInfoAtIndex);
						EnumIndex = 1;
						SlotVar = ESlotType::SecondSlot;
						if (!IsArmed)
							IsArmed = true;
						

				break;
			}
			case ESlotType::ThirdSlot:
			{
					WeaponInfoAtIndex = InventoryComponent->GetWeaponInfo(2);

						InitWeapon(WeaponInfoAtIndex);
						EnumIndex = 2;
						SlotVar = ESlotType::ThirdSlot;
						if (!IsArmed)
							IsArmed = true;
					
				break;
			}
			case ESlotType::FourthSlot:
			{

					WeaponInfoAtIndex = InventoryComponent->GetWeaponInfo(3);

						InitWeapon(WeaponInfoAtIndex);
						EnumIndex = 3;
						SlotVar = ESlotType::FourthSlot;
						if (!IsArmed)
							IsArmed = true;
					
				break;
			}

		}
	
	if (WeaponInfoAtIndex.WeaponClass)
	{		
	}
	else
	{
		EquipWeapon();
	}
	//Equip AnimCheck
	
	return SlotVar;
}



ESlotType AProjectXCharacter::GetCurrentSlot(int32& IndexOfEnum)
{
	IndexOfEnum = EnumIndex;
	
	return SlotVar;
}



void AProjectXCharacter::TryUseAbillity()
{
	if (InventoryComponent && InventoryComponent->isBracerEquiped)
	{
		ESkillList CurrentSkill = SkillComponent->CurrentSkill;
		
		switch (CurrentSkill)
		{
		case ESkillList::Teleport:
			SkillComponent->Teleport(InventoryComponent->CoolDown);
			break;
		case ESkillList::SlowMode:
			SkillComponent->SlowMode(InventoryComponent->CoolDown, InventoryComponent->TimeRemaining);
			break;
		case ESkillList::Recall:
			SkillComponent->Recall(InventoryComponent->CoolDown);
			break;
		case ESkillList::RageMode:
			IsBombDropped = false;
			SkillComponent->RageMode();
			break;
		case ESkillList::SnakeMode:
			//SkillComponent->Recall(InventoryComponent->CoolDown);
			break;
		default:
			break;
		}
	}
	 

	
	
	/*
	if (AbillityEffect)//to do colldown
	{
		UProjectX_StateEffect* NewEffect = NewObject<UProjectX_StateEffect>(this, AbillityEffect);
		if(NewEffect)
		{
			NewEffect->InitObject(this);
		}
	} */

}

EPhysicalSurface AProjectXCharacter::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;
	if (CharHealthComponent)
	{
		if (CharHealthComponent->GetCurrentShield() <= 0)
		{
			if (GetMesh())
			{
				UMaterialInterface* MyMaterial = GetMesh()->GetMaterial(0);
				if (MyMaterial)
				{
					Result = MyMaterial->GetPhysicalMaterial()->SurfaceType;
				}
			}
		}
		
	}

	
	return Result;
}

TArray<UProjectX_StateEffect*> AProjectXCharacter::GetAllCurrentEffects()
{
	return Effects;
}

void AProjectXCharacter::RemoveEffect(UProjectX_StateEffect* RemoveEffect)
{		
	Effects.Remove(RemoveEffect);
}

void AProjectXCharacter::AddEffect(UProjectX_StateEffect* NewEffect)
{
	

	Effects.Add(NewEffect);
}

void AProjectXCharacter::GetSpawnLocationForEffect(FVector& MeshLocation, FName& BoneName)
{
	if (GetMesh())
	{
		MeshLocation = EffectOffset;
		BoneName = BoneNameForEffect;		
	}
	
}


void AProjectXCharacter::TryToInterractWithObject()
{	
		OnIterractButtonPressed.Broadcast();		
}

void AProjectXCharacter::CharDead()
{
	int32 rnd = 0;
	float TimeAnim = 0.0f;
	rnd = FMath::RandHelper(DeadAnim.Num());
	if (DeadAnim[rnd] && DeadAnim.IsValidIndex(rnd) && GetMesh() && GetMesh()->GetAnimInstance())
	{
		TimeAnim = DeadAnim[rnd]->GetPlayLength()-0.3;
		GetMesh()->GetAnimInstance()->Montage_Play(DeadAnim[rnd]);
	}

	bIsAlive = false;

	if (GetController())
	{
		GetController()->UnPossess();
	}
	//UnPossessed();
	//TimerRagdooll
	GetWorldTimerManager().SetTimer(TimerHanndle_RagDollTimer, this, &AProjectXCharacter::EnableRagdoll, TimeAnim,false);

	GetCursorToWorld()->SetVisibility(false);
	
	//Подумать, как переделать, чтобы персонаж рандомно продолжал стрелять после смерти
		AttackCharEvent(false);


	CharDead_BP();
}

void AProjectXCharacter::EnableRagdoll()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
}

void AProjectXCharacter::EnableHealthRegenTimer(float HealthRegenSpeed)
{
	/* 
	if (CharHealthComponent->RegenDone)
	{
		GetWorldTimerManager().ClearTimer(RegenTimer);
	}
	else
	{
		GetWorldTimerManager().SetTimer(RegenTimer, this, &AProjectXCharacter::HealthRegen, HealthRegenSpeed, true, 0.1f);
	}
	*/
}

void AProjectXCharacter::HealthRegen()
{
	CharHealthComponent->HealthRegen();
}

float AProjectXCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bIsAlive)
	{
		CharHealthComponent->ChangeHealthValue(-DamageAmount);
	}

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		AProjectileDefault* myProjectile = Cast<AProjectileDefault>(DamageCauser);
		if (myProjectile)
		{
			UTypes::AddEffectBySurfaceType(this, myProjectile->ProjectileSetting.Effect, GetSurfaceType());
			//myProjectile->ProjectileSetting.Effect;
		}
	}

	return ActualDamage;
}
 
void AProjectXCharacter::CharDead_BP_Implementation()
{
	//bp only
}





