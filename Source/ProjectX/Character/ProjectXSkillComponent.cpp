// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectX/Character/ProjectXSkillComponent.h"
#include "ProjectX/Character/ProjectXCharacter.h"
#include "DrawDebugHelpers.h"
#include "ProjectX/Game/ProjectXGameMode.h"


// Sets default values for this component's properties
UProjectXSkillComponent::UProjectXSkillComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UProjectXSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	
	
}


// Called every frame
void UProjectXSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UProjectXSkillComponent::SkillIsEnable()
{
	if (CharIsDead)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SkillTimerForWidget);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_BonusSkillTimerForWidget);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SlowMoTimer);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CoolDownTimer);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CheckTimerValue);
		GetWorld()->GetTimerManager().ClearTimer(TimerHanlde_BonusSkillCooldown);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CheckBonusTimerValue);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_BonusSkillTimer);
		TimerForWIdgetUpdateInfo = 0.0f;
		BonusTimerForWIdgetUpdateInfo = 0.0f;
		isSkillOnCoolDown = false;
		SlowModeEnd();
		SnakeModeEnd();		
		BastionModeEnd();
		if(isRageModeOn)
			RageModeEnd();
	}
	else
	{
		if (isSkillOnCoolDown && TimerForWIdgetUpdateInfo <= 0.1f)
		{
			isSkillOnCoolDown = false;
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CoolDownTimer);
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CheckTimerValue);
			TimerForWIdgetUpdateInfo = 0.0f;
		}

		if (isBonusSkillOnCoolDown && BonusTimerForWIdgetUpdateInfo <= 0.1f)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHanlde_BonusSkillCooldown);
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CheckBonusTimerValue);
			isBonusSkillOnCoolDown = false;
			BonusTimerForWIdgetUpdateInfo = 0.0f;
		}
	}

}

//Значение таймера, для счета кд
void UProjectXSkillComponent::CheckTimeRemainingOnCoolDown()
{
	TimerForWIdgetUpdateInfo = GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_CoolDownTimer);
	OnTimerStarted.Broadcast(TimerForWIdgetUpdateInfo);
}

void UProjectXSkillComponent::CheckSkillTimer()
{
	SkillTimerForWidget = GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_SlowMoTimer);
	
	BonusSkillTimerForWidget = GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_BonusSkillTimer);
	
	OnSkillTimerCount.Broadcast(SkillTimerForWidget, BonusSkillTimerForWidget);
	if (SkillTimerForWidget <= 0 && BonusSkillTimerForWidget <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SkillTimerForWidget);
	}
}

//Инициализация таймера счетчика кд
void UProjectXSkillComponent::InitTimerRemainingCooldown()
{	
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckTimerValue, this, &UProjectXSkillComponent::CheckTimeRemainingOnCoolDown, 0.1f, true);	
}

void UProjectXSkillComponent::SwitchSkills()
{
	if (isBastionModeAvailable || isSnakeModeAvailable || isRageModeAvailable)
	{		
		ESkillList TempSkill = CurrentSkill;
		if (TempSkill == BonusSkill)
		{

		}
		else
		{
			CurrentSkill = BonusSkill;
			BonusSkill = TempSkill;
			OnSkillSwitched.Broadcast();
			UE_LOG(LogTemp, Warning, TEXT("SwitchSkill"));
		}
	}
}

void UProjectXSkillComponent::Teleport(float TeleportCoolDown)
{	
	if (isSkillOnCoolDown)
	{
		
	}
	else
	{
		FVector ActorLocation(0, 0, 0);
		ActorLocation = GetOwner()->GetActorLocation();
		if (StartTeleportPoint)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), StartTeleportPoint, GetOwner()->GetTransform());

		if (TeleportSound)
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), TeleportSound, ActorLocation);

		FHitResult Hit;
		FVector Forward = GetOwner()->GetActorForwardVector() * 1000 + ActorLocation;

		GetWorld()->LineTraceSingleByChannel(Hit, ActorLocation, Forward, ECollisionChannel::ECC_GameTraceChannel1, FCollisionQueryParams::DefaultQueryParam);

		DrawDebugLine(GetWorld(), ActorLocation, Forward, FColor::Red, false, 5.f, (uint8)'\000', 0.8f);
		if (Hit.bBlockingHit)
		{
			GetOwner()->SetActorLocation(Hit.Location);
			//UE_LOG(LogTemp, Warning, TEXT("Vector: X = %f. Y = %f. Size = %f"), ActorLocation.X, ActorLocation.Y, ActorLocation.Size());
		}
		else
		{
			GetOwner()->SetActorLocation(Forward);
		}

		isSkillOnCoolDown = true;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CoolDownTimer, this, &UProjectXSkillComponent::SkillIsEnable, TeleportCoolDown, false);
		InitTimerRemainingCooldown();
		if (EndTeleportPoint)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EndTeleportPoint, GetOwner()->GetTransform());
		}
		CoolDown = TeleportCoolDown;
		OnSkillUsed.Broadcast(ESkillList::Teleport);
	}
	
}

void UProjectXSkillComponent::SlowMode(float SlowMoCooldown, float TimerRemain)
{	
	if (isSkillOnCoolDown)
	{
		
	}
	else
	{
		isSkillOnCoolDown = true;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CoolDownTimer, this, &UProjectXSkillComponent::SkillIsEnable, SlowMoCooldown, false);
		InitTimerRemainingCooldown();

		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.5f);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SlowMoTimer, this, &UProjectXSkillComponent::SlowModeEnd, TimerRemain, false);
		
		//Снимаем значение с таймера
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SkillTimerForWidget, this, &UProjectXSkillComponent::CheckSkillTimer, 0.1f, true);
		CoolDown = SlowMoCooldown;
		OnSkillUsed.Broadcast(ESkillList::SlowMode);
	}
}

void UProjectXSkillComponent::SlowModeEnd()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

void UProjectXSkillComponent::CheckTimeRemainingOnBonusSkillCoolDown()
{
	BonusTimerForWIdgetUpdateInfo = GetWorld()->GetTimerManager().GetTimerRemaining(TimerHanlde_BonusSkillCooldown);
	OnBonusTimerStarted.Broadcast(BonusTimerForWIdgetUpdateInfo);
}

void UProjectXSkillComponent::ChoseSkill(ESkillList Skill)
{

		switch (CurrentSkill)
		{
		case ESkillList::RageMode:
			CurrentSkill = Skill;
			break;
		case ESkillList::SnakeMode:
			CurrentSkill = Skill;
			break;
		case ESkillList::BastionMode:
			CurrentSkill = Skill;
			break;
		default:
			break;
		}


		switch (BonusSkill)
		{
		case ESkillList::RageMode:
			BonusSkill = Skill;
			break;
		case ESkillList::SnakeMode:
			BonusSkill = Skill;
			break;
		case ESkillList::BastionMode:
			BonusSkill = Skill;
			break;
		default:
			break;
		

		}
	OnSkillSwitched.Broadcast();
}

void UProjectXSkillComponent::Recall(float RecallCoolDown)
{
	if (isSkillOnCoolDown)
	{
		
	}
	else
	{
		isSkillOnCoolDown = true;

		if (StartRecallPoint)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), StartRecallPoint, GetOwner()->GetTransform());

		if (RecallSound)
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), RecallSound, GetOwner()->GetActorLocation());

		APlayerController* PK = Cast<APlayerController>(this->GetOwner()->GetInstigatorController());
		AProjectXGameMode* Gamemode = Cast<AProjectXGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (Gamemode && PK)
		{
			AActor* PlayerStart = Gamemode->FindPlayerStart(PK, "Phase1");
			if (PlayerStart)
			{
				GetOwner()->SetActorLocation(PlayerStart->GetActorLocation());
			}
		}
		if (EndRecallPoint)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EndRecallPoint, GetOwner()->GetTransform());
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SkillTimerForWidget, this, &UProjectXSkillComponent::CheckSkillTimer, 0.1f, true);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CoolDownTimer, this, &UProjectXSkillComponent::SkillIsEnable, RecallCoolDown, false);
		InitTimerRemainingCooldown();
		CoolDown = RecallCoolDown;
		OnSkillUsed.Broadcast(ESkillList::Recall);
	}
	

}

void UProjectXSkillComponent::RageMode()
{
	if (isBonusSkillOnCoolDown)
	{

	}
	else
	{
		UProjectXHealthComponent* HealthComp = Cast<UProjectXHealthComponent>(GetOwner()->GetComponentByClass(UProjectXHealthComponent::StaticClass()));
		if (HealthComp)
		{
			HealthComp->CoefDamage -= 0.1;

			float TempMaxHealth = HealthComp->GetCurrentMaxHealth() * 1.2f;
			GetWorld()->GetTimerManager().SetTimer(TimerHanlde_BonusSkillCooldown, this, &UProjectXSkillComponent::SkillIsEnable, RageCoolDownTimer, false);
			
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckBonusTimerValue, this, &UProjectXSkillComponent::CheckTimeRemainingOnBonusSkillCoolDown, 0.1f, true);

			HealthComp->SetCurrentMaxHealth(TempMaxHealth);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_BonusSkillTimer, this, &UProjectXSkillComponent::RageModeEnd, RageTimer, false);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_SkillTimerForWidget, this, &UProjectXSkillComponent::CheckSkillTimer, 0.1f, true);
			isBonusSkillOnCoolDown = true;
			isRageModeOn = true;
			OnSkillUsed.Broadcast(ESkillList::RageMode);
		}
	}

	
}

void UProjectXSkillComponent::RageModeEnd()
{
	isRageModeOn = false;

	UProjectXHealthComponent* HealthComp = Cast<UProjectXHealthComponent>(GetOwner()->GetComponentByClass(UProjectXHealthComponent::StaticClass()));
	if (HealthComp)
	{
		HealthComp->CoefDamage += 0.5;

		float TempMaxHealth = HealthComp->GetCurrentMaxHealth() / 1.2f;

		HealthComp->SetCurrentMaxHealth(TempMaxHealth);
		
	}
}

void UProjectXSkillComponent::SetSkills(ESkillList Current, ESkillList Bonus)
{
	CurrentSkill = Current;
	BonusSkill = Bonus;
}

void UProjectXSkillComponent::SnakeMode()
{
	if (isBonusSkillOnCoolDown)
	{

	}
	else
	{
		AProjectXCharacter* Player = Cast<AProjectXCharacter>(GetOwner());
		if (Player)
		{
			UProjectXGameInstance* MyGI = Cast<UProjectXGameInstance>(GetWorld()->GetGameInstance());
			FWeaponInfo WeaponInfo;
			MyGI->GetWeaponInfoByName("SnakeModePistol", WeaponInfo);

			if(Player->GetCurrentWeapon())
				LastEquipedWeapon = Player->GetCurrentWeapon()->WeaponSetting;
			
			Player->isSnakeModeEnabled = true;
			Player->InitWeapon(WeaponInfo);
			
			if(StartSnakeModeAnim)
				Player->GetMesh()->GetAnimInstance()->Montage_Play(StartSnakeModeAnim);

			if (SnakeModeSound)
				UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SnakeModeSound, GetOwner()->GetActorLocation());

			GetWorld()->GetTimerManager().SetTimer(TimerHanlde_BonusSkillCooldown, this, &UProjectXSkillComponent::SkillIsEnable, SnakeCoolDownTimer, false);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckBonusTimerValue, this, &UProjectXSkillComponent::CheckTimeRemainingOnBonusSkillCoolDown, 0.1f, true);

			
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_BonusSkillTimer, this, &UProjectXSkillComponent::SnakeModeEnd, SnakeTimer, false);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_SkillTimerForWidget, this, &UProjectXSkillComponent::CheckSkillTimer, 0.1f, true);
			isSnakeModeOn = true;
			isBonusSkillOnCoolDown = true;
			OnSnakeModeStatus.Broadcast(isSnakeModeOn);
			OnSkillUsed.Broadcast(ESkillList::SnakeMode);
		}
	}
}


void UProjectXSkillComponent::SnakeModeEnd()
{
	AProjectXCharacter* Player = Cast<AProjectXCharacter>(GetOwner());
	if (Player)
	{		
		Player->isSnakeModeEnabled = false;
		Player->InitWeapon(LastEquipedWeapon);

		if (EndSnakeModeAnim)
			Player->GetMesh()->GetAnimInstance()->Montage_Play(EndSnakeModeAnim);
		isSnakeModeOn = false;
		OnSnakeModeStatus.Broadcast(isSnakeModeOn);
	}
}
void UProjectXSkillComponent::BastionMode()
{
	if (isBonusSkillOnCoolDown)
	{

	}
	else
	{
		UProjectXCharacterHealthComponent* HealthComp = Cast<UProjectXCharacterHealthComponent>(GetOwner()->GetComponentByClass(UProjectXCharacterHealthComponent::StaticClass()));
		if (HealthComp)
		{
			HealthComp->SetShieldValue(BastionShieldValue);

			GetWorld()->GetTimerManager().SetTimer(TimerHanlde_BonusSkillCooldown, this, &UProjectXSkillComponent::SkillIsEnable, BastionCoolDownTimer, false);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckBonusTimerValue, this, &UProjectXSkillComponent::CheckTimeRemainingOnBonusSkillCoolDown, 0.1f, true);

			
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_BonusSkillTimer, this, &UProjectXSkillComponent::BastionModeEnd, BastionTimer, false);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_SkillTimerForWidget, this, &UProjectXSkillComponent::CheckSkillTimer, 0.1f, true);
			
			isBonusSkillOnCoolDown = true;
			OnSkillUsed.Broadcast(ESkillList::BastionMode);
		}
	}
}


void UProjectXSkillComponent::BastionModeEnd()
{
	UProjectXCharacterHealthComponent* HealthComp = Cast<UProjectXCharacterHealthComponent>(GetOwner()->GetComponentByClass(UProjectXCharacterHealthComponent::StaticClass()));
	if (HealthComp)
	{
		HealthComp->SetShieldValue(0);
	}
	
}
