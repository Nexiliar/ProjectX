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

bool UProjectX_StateEffect_ExecuteTimer::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);
	//UE_LOG(LogTemp, Warning, TEXT("UProjectX_StateEffect_ExecuteTimer: FIRE"));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectTimer, this, &UProjectX_StateEffect_ExecuteTimer::DestroyObject, Timer, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UProjectX_StateEffect_ExecuteTimer::Execute, RateTime, true);

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
				UE_LOG(LogTemp, Warning, TEXT("BoneLocation %s"), *Loc.ToString());
				UE_LOG(LogTemp, Warning, TEXT("BoneName %s"), *NameBoneToAttach.ToString());
				ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, myActor->GetRootComponent()->GetChildComponent(1), NameBoneToAttach, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
			}
			else
			{
			
				ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, myActor->GetRootComponent(), NameBoneToAttach, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
				UE_LOG(LogTemp, Warning, TEXT("BoneName %s"), *NameBoneToAttach.ToString());
			}

			
		}
		
		//to do for object with interface
		

		//ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, myActor->GetComponentByClass(USkeletalMesh::StaticClass()), NameBoneToAttach, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
		
	}
	
	return true;
}

void UProjectX_StateEffect_ExecuteTimer::DestroyObject()
{
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
