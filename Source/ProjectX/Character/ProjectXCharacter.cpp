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
	
	if (CharHealthComponent)
	{
		CharHealthComponent->OnDead.AddDynamic(this, &AProjectXCharacter::CharDead);
	}

	/* 
	if (InventoryComponent && IsArmed)
	{
		SwitchWeaponToSlotNum(ESlotType::FirstSlot);
		InventoryComponent->OnSwitchWeapon.AddDynamic(this, &AProjectXCharacter::InitWeapon);		
	}
	*/
	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AProjectXCharacter::BeginPlay()
{
	Super::BeginPlay();

	//InitWeapon(InitWeaponName);

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
	
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, TEXT("HI THERE"));
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

	NewInputComponent->BindAction(TEXT("SwitchNextWeapon"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::TrySwicthNextWeapon);
	NewInputComponent->BindAction(TEXT("SwitchPrevioseWeapon"), EInputEvent::IE_Pressed, this, &AProjectXCharacter::TrySwitchPreviosWeapon);
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



void AProjectXCharacter::InputAxisY(float Value)
{
	AxisY = Value;
}

void AProjectXCharacter::InputAttackPressed()
{
	AttackCharEvent(true);
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

					//���������� ����� � ������� ������� ����
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
		else
			UE_LOG(LogTemp, Warning, TEXT("ATPSCharacter::AttackCharEvent - CurrentWeapon -NULL"));
}

	void AProjectXCharacter::CharacterUpdate()
{
	float ResSpeed = 600.0f;
	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResSpeed = MovementSpeedInfo.AimSpeedNormal;
		break;
	case EMovementState::AimWalk_State:
		ResSpeed = MovementSpeedInfo.AimSpeedWalk;
		break;
	case EMovementState::AimCrouch_State:
		ResSpeed = MovementSpeedInfo.AimCrouchSpeed;
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementSpeedInfo.WalkSpeedNormal;
		break;
	case EMovementState::Crouch_State:
		ResSpeed = MovementSpeedInfo.CrouchSpeedNormal;
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementSpeedInfo.RunSpeedNormal;
		break;
	case EMovementState::SprintRun_State:
		ResSpeed = MovementSpeedInfo.SprintRunSpeedRun;
		break;
	default:
		break;
	}



	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;

}

void AProjectXCharacter::ChangeMovementState()

{
	
	FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
	if (!WalkEnabled && !SprintRunEnabled && !AimEnabled && !CrouchEnabled)
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
		if (CrouchEnabled && !WalkEnabled && !SprintRunEnabled && !AimEnabled)
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
		if (AimEnabled && CrouchEnabled && !WalkEnabled && !SprintRunEnabled)
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
		if (SprintRunEnabled)
		{
			WalkEnabled = false;
			AimEnabled = false;
			MovementState = EMovementState::SprintRun_State;
			
		}
		if (WalkEnabled && !SprintRunEnabled && AimEnabled && !CrouchEnabled)
		{
			MovementState = EMovementState::AimWalk_State;
			if (CurrentWeapon)
			CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
		}
		else
		{
			if (WalkEnabled && !SprintRunEnabled && !AimEnabled && !CrouchEnabled)
			{
				MovementState = EMovementState::Walk_State;
				if (CurrentWeapon)
				CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketNoAim"));
			}
			else
			{
				if (!WalkEnabled && !SprintRunEnabled && AimEnabled && !CrouchEnabled)
				{
					MovementState = EMovementState::Aim_State;
					if (CurrentWeapon)
					CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
				}
			}
		}
	}
	CharacterUpdate();
	//Weapon state update
	AWeaponDefault* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		
		myWeapon->UpdateStateWeapon(MovementState);
	}
}

AWeaponDefault* AProjectXCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}
//���������� ������ �������� � ����������� ������. ���� ��� �����, ��������� ������
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

void AProjectXCharacter::InitWeapon(FName IdWeaponName, FAddicionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon)//ToDo Init by id row by table

{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
	UProjectXGameInstance* MyGI = Cast<UProjectXGameInstance>(GetGameInstance());
	FWeaponInfo myWeaponInfo;

	if (MyGI)
	{
		if (MyGI->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))
		{
		
			if (myWeaponInfo.WeaponClass)
			{
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myWeapon)
				{
					
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
					CurrentWeapon = myWeapon;
					
					

					
					myWeapon->WeaponSetting = myWeaponInfo;
					//myWeapon->WeaponInfo.Round = myWeapon->WeaponSetting.MaxRound;
					//Remove DEBUG!
					myWeapon->ReloadTime = myWeaponInfo.ReloadTime;
					myWeapon->UpdateStateWeapon(MovementState);
					
					myWeapon->WeaponInfo = WeaponAdditionalInfo;
					//if (InventoryComponent)
						CurrentIndexWeapon = InventoryComponent->GetWeaponIndexSlotByName(IdWeaponName);
					//������� ���������, ����������� � weapondefault
					myWeapon->OnWeaponReloadStart.AddDynamic(this, &AProjectXCharacter::WeaponReloadStart);
					myWeapon->OnWeaponReloadEnd.AddDynamic(this, &AProjectXCharacter::WeaponReloadEnd);
					myWeapon->OnWeaponFireStart.AddDynamic(this, &AProjectXCharacter::WeaponFireStart);

					//�������� ��������������, ����� ������� ������
					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
						CurrentWeapon->InitReload();
					/* 
					int32  SlotIndex;
					ESlotType TempSlotInfo = GetCurrentSlot(SlotIndex);

					if (InventoryComponent)
						InventoryComponent->OnWeaponInit.Broadcast(InventoryComponent->WeaponSlots[SlotIndex].AdditionalInfo, InventoryComponent->WeaponSlots[SlotIndex].NameItem);
					
					*/
					
				
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AProjectXCharacter::InitWeapon - Weapon not found in table -NULL"));
		}
	}
	

	
}

void AProjectXCharacter::TryReloadWeapon()
{
	if (CurrentWeapon && !CurrentWeapon->WeaponReloading)
	{
		if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound && CurrentWeapon->CheckCanWeaponReload())
			
			CurrentWeapon->InitReload();
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
		InventoryComponent->AmmoSlotChangeValue(CurrentWeapon->WeaponSetting.WeaponType, AmmoTake);
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->WeaponInfo);
	}
	WeaponReloadEnd_BP(bIsSucces);
}

void AProjectXCharacter::WeaponFireStart(UAnimMontage* Anim)
{
	if (InventoryComponent && CurrentWeapon)
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon,CurrentWeapon->WeaponInfo);
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
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, TEXT("Equiped"));

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
		
		//InventoryComponent->OnSwitchWeapon.AddDynamic(this, &AProjectXCharacter::InitWeapon);

		IsArmed = true;
	}
}

ESlotType AProjectXCharacter::SwitchWeaponToSlotNum(ESlotType EquipmentSlotNumber)
{
	FName FirstIndexWeaponName;
	//if (CurrentWeapon && !CurrentWeapon->WeaponReloading)
	
		switch (EquipmentSlotNumber)
		{
			case ESlotType::FirstSlot:
			{
				FirstIndexWeaponName = InventoryComponent->GetWeaponNameBySlotIndex(0);
				FAddicionalWeaponInfo FirstIndexWeaponInfo = InventoryComponent->GetAdditionalInfoWeapon(0);
				int32 FirstIndexWeapon = InventoryComponent->GetWeaponIndexSlotByName(FirstIndexWeaponName);
				InitWeapon(FirstIndexWeaponName, FirstIndexWeaponInfo, FirstIndexWeapon);

				EnumIndex = 0;
				SlotVar = ESlotType::FirstSlot;
				if (!IsArmed)
					IsArmed = true;

				break;
			}
			case ESlotType::SecondSlot:
			{
				FirstIndexWeaponName = InventoryComponent->GetWeaponNameBySlotIndex(1);
				FAddicionalWeaponInfo FirstIndexWeaponInfo = InventoryComponent->GetAdditionalInfoWeapon(1);
				int32 FirstIndexWeapon = InventoryComponent->GetWeaponIndexSlotByName(FirstIndexWeaponName);
				InitWeapon(FirstIndexWeaponName, FirstIndexWeaponInfo, FirstIndexWeapon);
				EnumIndex = 1;
				SlotVar = ESlotType::SecondSlot;
				if (!IsArmed)
					IsArmed = true;

				break;
			}
			case ESlotType::ThirdSlot:
			{
				FirstIndexWeaponName = InventoryComponent->GetWeaponNameBySlotIndex(2);
				FAddicionalWeaponInfo FirstIndexWeaponInfo = InventoryComponent->GetAdditionalInfoWeapon(2);
				int32 FirstIndexWeapon = InventoryComponent->GetWeaponIndexSlotByName(FirstIndexWeaponName);
				InitWeapon(FirstIndexWeaponName, FirstIndexWeaponInfo, FirstIndexWeapon);
				EnumIndex = 2;
				SlotVar = ESlotType::ThirdSlot;
				if (!IsArmed)
					IsArmed = true;

				break;
			}
			case ESlotType::FourthSlot:
			{
				FirstIndexWeaponName = InventoryComponent->GetWeaponNameBySlotIndex(3);
				FAddicionalWeaponInfo FirstIndexWeaponInfo = InventoryComponent->GetAdditionalInfoWeapon(3);
				int32 FirstIndexWeapon = InventoryComponent->GetWeaponIndexSlotByName(FirstIndexWeaponName);
				InitWeapon(FirstIndexWeaponName, FirstIndexWeaponInfo, FirstIndexWeapon);
				EnumIndex = 3;
				SlotVar = ESlotType::FourthSlot;
				if (!IsArmed)
					IsArmed = true;

				//GLog->Log("Equipped FourthSlot");
				break;
			}

		}

	
	if (FirstIndexWeaponName == "None")
	{
		EquipWeapon();
	}

	if (CurrentWeapon && CurrentWeapon->WeaponSetting.WeaponType == EWeaponType::Pistol)
	{
		IsRifle = false;
	}
	else
	{
		IsRifle = true;
	}
	int32  SlotIndex;
	ESlotType TempSlotInfo = GetCurrentSlot(SlotIndex);
	if (InventoryComponent)
		InventoryComponent->OnWeaponInit.Broadcast(InventoryComponent->WeaponSlots[SlotIndex].AdditionalInfo, InventoryComponent->WeaponSlots[SlotIndex].NameItem);
	

	return SlotVar;
}



ESlotType AProjectXCharacter::GetCurrentSlot(int32& IndexOfEnum)
{
	IndexOfEnum = EnumIndex;
	
	return SlotVar;


}

void AProjectXCharacter::TrySwicthNextWeapon()
{
	if (InventoryComponent->WeaponSlots.Num() > 1)
	{
		//We have more then one weapon go switch
		int8 OldIndex = CurrentIndexWeapon;
		FAddicionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->WeaponInfo;
			if (CurrentWeapon->WeaponReloading)
				CurrentWeapon->CancelReload();
		}

		if (InventoryComponent)
		{
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon + 1, OldIndex, OldInfo, true))
			{
			}
		}
	}
}

void AProjectXCharacter::TrySwitchPreviosWeapon()
{
	if (InventoryComponent->WeaponSlots.Num() > 1)
	{
		//We have more then one weapon go switch
		int8 OldIndex = CurrentIndexWeapon;
		FAddicionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->WeaponInfo;
			if (CurrentWeapon->WeaponReloading)
				CurrentWeapon->CancelReload();
		}

		if (InventoryComponent)
		{
			
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo,false))
			{
			}
		}
	}
}

void AProjectXCharacter::TryUseAbillity()
{
	if (AbillityEffect)//to do colldown
	{
		UProjectX_StateEffect* NewEffect = NewObject<UProjectX_StateEffect>(this, AbillityEffect);
		if(NewEffect)
		{
			NewEffect->InitObject(this);
		}
	}
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





