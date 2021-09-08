// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectX/Game/GameObj/ProjectX_DebuffZone.h"

// Sets default values
AProjectX_DebuffZone::AProjectX_DebuffZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SceneComponent->SetupAttachment(RootComponent);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	BoxComponent->SetupAttachment(SceneComponent);

	ZoneFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Zone FX"));
	ZoneFX->SetupAttachment(SceneComponent);
}

// Called when the game starts or when spawned
void AProjectX_DebuffZone::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectX_DebuffZone::CollisionBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AProjectX_DebuffZone::CollisionBoxEndOverlap);

}

// Called every frame
void AProjectX_DebuffZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectX_DebuffZone::CollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AProjectXCharacter* Player = Cast<AProjectXCharacter>(OtherActor);

		if (Player)
		{			
			UTypes::AddDebuffEffect(Player, Effect);
		}		

	//UE_LOG(LogTemp, Warning, TEXT("DebuffZoneOVerlap"));
}

void AProjectX_DebuffZone::CollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("DebuffZoneSTOPOVerlap"));
}

