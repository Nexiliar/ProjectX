// Fill out your copyright notice in the Description page of Project Settings.

#include "DrawDebugHelpers.h"
#include "ProjectX/Game/ProjectXGameMode.h"
#include "ProjectX/Character/ProjectXSkillComponent.h"

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
	isSkillOnCoolDown = false;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CoolDownTimer);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CheckTimerValue);
	TimerForWIdgetUpdateInfo = 0.0f;
}

void UProjectXSkillComponent::CheckTimeRemainingOnCoolDown()
{
	TimerForWIdgetUpdateInfo = GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_CoolDownTimer);
	OnTimerStarted.Broadcast(TimerForWIdgetUpdateInfo);
}

void UProjectXSkillComponent::InitTimerRemainingCooldown()
{	
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckTimerValue, this, &UProjectXSkillComponent::CheckTimeRemainingOnCoolDown, 0.1f, true);	
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

