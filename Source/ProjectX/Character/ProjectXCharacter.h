// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "Math/Vector.h"
#include "ProjectX/Weapons/ProjectX_StateEffect.h"
#include "ProjectX/Weapons/WeaponDefault.h"
#include "ProjectX/Character/ProjectXInventoryComponent.h"
#include "ProjectX/FunctionLibrary/Types.h"
#include "ProjectX/Character/ProjectXCharacterHealthComponent.h"
#include "ProjectX/Interfaces/ProjectX_Interface_GameActor.h"

#include "ProjectXCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIterractButtonPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnequip);



UCLASS(Blueprintable)
class AProjectXCharacter : public ACharacter, public IProjectX_Interface_GameActor
{
	GENERATED_BODY()

public:
	AProjectXCharacter();

	FTimerHandle TimerHanndle_RagDollTimer;

	FTimerHandle RegenTimer;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnIterractButtonPressed OnIterractButtonPressed;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnWeaponUnequip WeaponUnequip;


	virtual void BeginPlay() override;
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	
	//Переопределение функции ввода
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;


	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	//FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawn, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* DropSpawnSphere;

	/** A decal that projects to the cursor location. */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UDecalComponent* CursorToWorld;

	FTimerHandle DelayTimer;
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UProjectXInventoryComponent* InventoryComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UProjectXCharacterHealthComponent* CharHealthComponent;
	
	
	//Cursor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
		UMaterialInterface* CursorMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
		FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);
	//Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		EMovementState MovementState = EMovementState::Run_State;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FCharacterSpeed MovementSpeedInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool SprintRunEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool CrouchEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool WalkEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool AimEnabled = false;
	UPROPERTY()
		bool isEqual;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool SprintCheck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool bIsAlive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		TArray<UAnimMontage*> DeadAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abillity")
		TSubclassOf<UProjectX_StateEffect> AbillityEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EquipmentAnim")
		bool IsRifle = false;
	//??Delete or?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		FName BoneNameForEffect = "foot_r";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
		FVector EffectOffset;

	
	//Weapon	
	AWeaponDefault* CurrentWeapon = nullptr;
	
	UDecalComponent* CurrentCursor = nullptr;

	//Effect
	TArray<UProjectX_StateEffect*> Effects;
	


	UPROPERTY()
	bool isCharacterOverlapingItem = false;

	ESlotType SlotVar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponEnum")
	int32 EnumIndex = 0;
	//inputs
	UFUNCTION()
		void InputAxisY(float Value);
	UFUNCTION()
		void InputAxisX(float Value);
	UFUNCTION()
		void InputAttackPressed();
	UFUNCTION()
		void InputAttackReleased();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimLogic")
	bool IsArmed;


	
	float AxisX = 0.0f;
	float AxisY = 0.0f;
	// Tick Func
	UFUNCTION()
		void MovementTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
		void AttackCharEvent(bool bIsFiring);
	UFUNCTION(BlueprintCallable)
		void CharacterUpdate();
	UFUNCTION(BlueprintCallable)
		void ChangeMovementState();
	UFUNCTION(BlueprintCallable)
		void Compare();
	
	UFUNCTION(BlueprintCallable)
		UDecalComponent* GetCursorToWorld();
	UFUNCTION(BlueprintCallable)
		AWeaponDefault* GetCurrentWeapon();
	UFUNCTION(BlueprintCallable)
		void InitWeapon(FWeaponInfo InfoOfWeaponToInit);
	UFUNCTION(BlueprintCallable)
		void TryReloadWeapon();
	UFUNCTION()
		void WeaponReloadStart(UAnimMontage*Anim);
	UFUNCTION()
		void WeaponReloadEnd(bool bIsSucces, int32 AmmoSafe);
	UFUNCTION()
		void WeaponFireStart(UAnimMontage* Anim);
	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void WeaponReloadStart_BP(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void WeaponReloadEnd_BP(bool bIsSucces);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void WeaponFireStart_BP(UAnimMontage* Anim);

	UFUNCTION(BlueprintCallable)
		void EquipWeapon();


	//invetntory
	UFUNCTION(BlueprintCallable)
		ESlotType SwitchWeaponToSlotNum(ESlotType EquipmentSlotNumber);

	template<ESlotType EquipmentSlotNumber>
	void SwitchWeaponToSlotNum() { SwitchWeaponToSlotNum(EquipmentSlotNumber); }

	UFUNCTION(BlueprintCallable)
	ESlotType GetCurrentSlot(int32& IndexOfEnum);
	
	//OldSwitchSystem Del if not used
	//void TrySwicthNextWeapon();
	//void TrySwitchPreviosWeapon();
	
	UFUNCTION(BlueprintCallable)
		void TryUseAbillity();
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		int32 CurrentIndexWeapon = 0;

	//Interface
	EPhysicalSurface GetSurfaceType() override;
	TArray<UProjectX_StateEffect*> GetAllCurrentEffects() override;
	 void RemoveEffect(UProjectX_StateEffect* RemoveEffect) override;
	 void AddEffect(UProjectX_StateEffect* NewEffect) override;
	 void GetSpawnLocationForEffect(FVector& MeshLocation, FName& BoneName) override;
	//End Interface


	UFUNCTION(BlueprintCallable)
		void TryToInterractWithObject();
	UFUNCTION()
	void CharDead();
	void EnableRagdoll();
	
	UFUNCTION()
	void EnableHealthRegenTimer(float HealthRegenSpeed);
	
	UFUNCTION()
	void HealthRegen();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintNativeEvent)
		void CharDead_BP();

};

