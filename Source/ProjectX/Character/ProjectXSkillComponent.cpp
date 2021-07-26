// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectX/Character/ProjectXSkillComponent.h"
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
	if (isSkillOnCoolDown && TimerForWIdgetUpdateInfo<=0.1f)
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

//Значение таймера, для счета кд
void UProjectXSkillComponent::CheckTimeRemainingOnCoolDown()
{
	TimerForWIdgetUpdateInfo = GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_CoolDownTimer);
	OnTimerStarted.Broadcast(TimerForWIdgetUpdateInfo);
}
//Инициализация таймера счетчика кд
void UProjectXSkillComponent::InitTimerRemainingCooldown()
{	
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckTimerValue, this, &UProjectXSkillComponent::CheckTimeRemainingOnCoolDown, 0.1f, true);	
}

void UProjectXSkillComponent::SwitchSkills()
{
	ESkillList TempSkill = CurrentSkill;
	CurrentSkill = BonusSkill;
	BonusSkill = TempSkill;
	OnSkillSwitched.Broadcast();
}

void UProjectXSkillComponent::Teleport(float TeleportCoolDown)
{	
	if (isSkillOnCoolDown)
	{
		UE_LOG(LogTemp, Warning, TEXT("UProjectXSkillComponent::Teleport COOLDOWN"));
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
		CoolDown = SlowMoCooldown;
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
		
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CoolDownTimer, this, &UProjectXSkillComponent::SkillIsEnable, RecallCoolDown, false);
		InitTimerRemainingCooldown();
		CoolDown = RecallCoolDown;
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
			HealthComp->CoefDamage -= 0.5;

			float TempMaxHealth = HealthComp->GetCurrentMaxHealth() * 1.2f;
			GetWorld()->GetTimerManager().SetTimer(TimerHanlde_BonusSkillCooldown, this, &UProjectXSkillComponent::SkillIsEnable, RageModeTimer, false);
			
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckBonusTimerValue, this, &UProjectXSkillComponent::CheckTimeRemainingOnBonusSkillCoolDown, 0.1f, true);

			HealthComp->SetCurrentMaxHealth(TempMaxHealth);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_RageTimer, this, &UProjectXSkillComponent::RageModeEnd, 30, false);
			isBonusSkillOnCoolDown = true;
			isRageModeOn = true;
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

void UProjectXSkillComponent::BastionMode()
{
	if (isBonusSkillOnCoolDown)
	{

	}
	else
	{
		UProjectXHealthComponent* HealthComp = Cast<UProjectXHealthComponent>(GetOwner()->GetComponentByClass(UProjectXHealthComponent::StaticClass()));
		if (HealthComp)
		{




			isBastionModeAvailable = true;
			isBonusSkillOnCoolDown = true;
		}
	}
}

void UProjectXSkillComponent::BastionModeEnd()
{

}
