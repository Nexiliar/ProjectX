// Fill out your copyright notice in the Description page of Project Settings.



#include "ProjectX_StateEffect.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectX/Interfaces/ProjectX_Interface_GameActor.h"
#include "ProjectX/Character/ProjectXHealthComponent.h"




bool UProjectX_StateEffect::InitObject(AActor* Actor)
{
	
	myActor = Actor;
	

	IProjectX_Interface_GameActor* myInterface = Cast<IProjectX_Interface_GameActor>(myActor);
	if (myInterface)
	{
		myInterface->AddEffect(this);
	}
	return true;
}

void UProjectX_StateEffect::DestroyObject()
{
	IProjectX_Interface_GameActor* myInterface = Cast<IProjectX_Interface_GameActor>(myActor);
	if (myInterface)
	{
		myInterface->RemoveEffect(this);
	}
	
	myActor = nullptr;
	if (this && this->IsValidLowLevel())
	{
		this->ConditionalBeginDestroy();
	}
	
}



bool UProjectX_StateEffect_ExecuteOnce::InitObject(AActor* Actor)
{
	
	 Super::InitObject(Actor);
	ExecuteOnce();
	return true;
}

void UProjectX_StateEffect_ExecuteOnce::DestroyObject()
{
	Super::DestroyObject();
}

void UProjectX_StateEffect_ExecuteOnce::ExecuteOnce()
{
	if (myActor)
	{
		UProjectXHealthComponent* myHealthComp = Cast<UProjectXHealthComponent>(myActor->GetComponentByClass(UProjectXHealthComponent::StaticClass()));
		if (myHealthComp)
		{
			myHealthComp->ChangeHealthValue(Power);
		}
	}
	
	DestroyObject();
}

void UProjectX_StateEffect_ExecuteTimer::PostInitProperties()
{
	Super::PostInitProperties();

	//Вызываем только в игре, когда есть мир. В редакторе BeginPlay вызван не будет
	if (GetOuter() && GetOuter()->GetWorld())
		BeginPlay();
}

bool UProjectX_StateEffect_ExecuteTimer::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);
	//UE_LOG(LogTemp, Warning, TEXT("UProjectX_StateEffect_ExecuteTimer: FIRE"));
	if (isInterrupted)
	{
		//Timer = RateTime;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UProjectX_StateEffect_ExecuteTimer::Execute, RateTime, true);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_WidgetInfo, this, &UProjectX_StateEffect_ExecuteTimer::TimeRemaining, 0.1f, true);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectTimer, this, &UProjectX_StateEffect_ExecuteTimer::DestroyObject, Timer, false);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UProjectX_StateEffect_ExecuteTimer::Execute, RateTime, true);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_WidgetInfo, this, &UProjectX_StateEffect_ExecuteTimer::TimeRemaining, 0.1f, true);
	}





	if (ParticleEffect)
	{	
		FName NameBoneToAttach;		
		FVector Loc;
		
		FVector Normal;
		IProjectX_Interface_GameActor* myInterface = Cast<IProjectX_Interface_GameActor>(myActor);
		if (myInterface)
		{
			myInterface->GetSpawnLocationForEffect(Loc, NameBoneToAttach);
		 	if (NameBoneToAttach != "None")
			{
				//UE_LOG(LogTemp, Warning, TEXT("BoneLocation %s"), *Loc.ToString());
				//UE_LOG(LogTemp, Warning, TEXT("BoneName %s"), *NameBoneToAttach.ToString());
				ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, myActor->GetRootComponent()->GetChildComponent(1), NameBoneToAttach, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
			}
			else
			{
			
				ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, myActor->GetRootComponent(), NameBoneToAttach, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
				//UE_LOG(LogTemp, Warning, TEXT("BoneName %s"), *NameBoneToAttach.ToString());
			}

			
		}
		
		//to do for object with interface
		

		//ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, myActor->GetComponentByClass(USkeletalMesh::StaticClass()), NameBoneToAttach, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
		
	}
	
	return true;
}

void UProjectX_StateEffect_ExecuteTimer::DestroyObject()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_EffectTimer);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ExecuteTimer);
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_WidgetInfo);

	WidgetDebuffTimer = 0.0f;
	AdditionalWidgetDebuffTimer = 0.0f;
	OnDebuffTimeStarted.Broadcast(WidgetDebuffTimer, AdditionalWidgetDebuffTimer);

	if (ParticleEmitter)
	{
		ParticleEmitter->DestroyComponent();
		ParticleEmitter = nullptr;
		Super::DestroyObject();
	}

	

}





void UProjectX_StateEffect_ExecuteTimer::Execute()
{
	if (myActor)
	{
		UProjectXHealthComponent* myHealthComp = Cast<UProjectXHealthComponent>(myActor->GetComponentByClass(UProjectXHealthComponent::StaticClass()));
		if (myHealthComp)
		{
			myHealthComp->ChangeHealthValue(Power);
		}
	}

}



void UProjectX_StateEffect_ExecuteTimer::TimeRemaining()
{
	if (isInterrupted)
	{
		WidgetDebuffTimer = 1;
	}
	else
	{
		WidgetDebuffTimer = GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_EffectTimer);

		if (WidgetDebuffTimer <= 0.1f)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_WidgetInfo);
			WidgetDebuffTimer = 0.0f;
		}
	}


	//if (AdditionalWidgetDebuffTimer <= 0.1f)
	//{
	//	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_AdditionalWidgetInfo);
	//	AdditionalWidgetDebuffTimer = 0.0f;
	//}
	OnDebuffTimeStarted.Broadcast(WidgetDebuffTimer, AdditionalWidgetDebuffTimer);
}
