// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectX/Character/ProjectXCharacter.h"
#include "ProjectX/Character/ProjectXStatsComponent.h"


// Sets default values for this component's properties
UProjectXStatsComponent::UProjectXStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UProjectXStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	//OnStatsChange.Broadcast(SkillPoints, AttributePoints, CurrentLevel);
	OnStatsChange.Broadcast(CharacterStatistic, CurrentLevel, SkillPoints, AttributePoints);
	//StatDependance();
	// ...
	BasicCompsInit();
	ConRiseResult(true);
	ReactionRiseResult();
}


// Called every frame
void UProjectXStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
//Function to gain expirience for level up
void UProjectXStatsComponent::ExpGain(float Amount)
{
	CurrentExpirience = CurrentExpirience + Amount;
	float CurrentLeveExpirience;
	GetExpInfo(CurrentLeveExpirience);
	 
	if (CurrentExpirience > CurrentLeveExpirience)
	{
		float AmountExpLeft = CurrentExpirience - CurrentLeveExpirience;
		CurrentExpirience = AmountExpLeft;
		LevelUpEvent();	
		
	}
	else if (CurrentExpirience == CurrentLeveExpirience)
	{
		CurrentExpirience = 0;
		LevelUpEvent();		
	}
	
	
}
//Function to count how much exp do you have and witch level are you. Also how many exp you should collect to levelup
float UProjectXStatsComponent::GetExpInfo(float& CurrentMaxExp)
{
	for (int i = 0; i < CurrentLevel; i++)
	{
		LevelExpirience = AmountOfExpirienceNeedForLvlUpPerLevel[i];
		CurrentMaxExp = LevelExpirience;
	}
	
	return CurrentExpirience;
}



void UProjectXStatsComponent::LevelUpEvent()
{
	CurrentLevel = CurrentLevel + 1;
	SkillPoints = SkillPoints + 1;
	AttributePoints = AttributePoints + 1;

	
	Inventory->MaxWeightLimit += 5;
	OnStatsChange.Broadcast(CharacterStatistic, CurrentLevel, SkillPoints, AttributePoints);

	
}

UProjectXHealthComponent* UProjectXStatsComponent::GetHealthComponent()
{
	return HealthComponent;
}

bool UProjectXStatsComponent::UseSkillPoints()
{
	bool bIsSuccess = false;
	if (SkillPoints > 0)
	{
		SkillPoints = SkillPoints - 1;
		bIsSuccess = true;
	}
	return bIsSuccess;
}
bool UProjectXStatsComponent::UseAttributePoints()
{
	bool bIsSuccess = false;
	if (AttributePoints > 0)
	{
		AttributePoints = AttributePoints - 1;
		bIsSuccess = true;
	}
	return bIsSuccess;
}
void UProjectXStatsComponent::ConRiseResult(bool Init)
{		
	if (Init)
	{
		if (HealthComponent)
		{
			float ToChangeHealthValue = CharacterStatistic.Constitution * 10;
			HealthComponent->SetCurrentMaxHealth(ToChangeHealthValue);
		}
	}
	else
	{
		if (HealthComponent)
		{
			if (CharacterStatistic.Constitution == 10)
			{
				HealthComponent->CoefDamage -= 0.1;
			}
			if (CharacterStatistic.Constitution == 15)
			{
				HealthComponent->CoefDamage -= 0.1;
				HealthComponent->AddMaxHealthValue(30);
			}
			if (CharacterStatistic.Constitution == 20)
			{
				//Skill
			}
		}
		if (Inventory)
		{
			Inventory->MaxWeightLimit += 2;
			Inventory->OnCurrentWeightChange.Broadcast(Inventory->CurrentWeight, Inventory->MaxWeightLimit);
		}
	}
		

	

}
void UProjectXStatsComponent::StrRiseResult()
{
	//DO INIT FROM START
	if (Inventory)
	{
		Inventory->MaxWeightLimit += 2;
		Inventory->OnCurrentWeightChange.Broadcast(Inventory->CurrentWeight, Inventory->MaxWeightLimit);
		
		if (CharacterStatistic.Strength == 10)
		{
			IsSlotUnlocked();
		}
		if (CharacterStatistic.Strength == 15)
		{
			IsSlotUnlocked();
		}
		if (CharacterStatistic.Strength == 20)
		{
			IsSlotUnlocked();
			HealthComponent->CoefDamage -= 0.5f;
			//Skill
		}
	}

}
bool UProjectXStatsComponent::IsSlotUnlocked()
{
	int8 i = 0;
	bool bIsFound = false;
	while (i < Inventory->WeaponSlotsInfo.Num() && !bIsFound)
	{
		if (!Inventory->WeaponSlotsInfo[i].CanUseSlot)
		{
			Inventory->WeaponSlotsInfo[i].CanUseSlot = true;
			bIsFound = true;
		}
		i++;
	}
	return bIsFound;
}
void UProjectXStatsComponent::ReactionRiseResult()
{
	//DO INIT FROM START
	AProjectXCharacter* Player = Cast<AProjectXCharacter>(GetOwner());
	if (Player)
	{
		Player->CorrectAccuracyOnStatUp += 0.1f;
		Player->CorrectFireRateOnStatUp += 0.004f;
	}
}
FStatsInfo UProjectXStatsComponent::GetEveryStat(int32& level, int32& skillpoints, int32& att)
{
	level = CurrentLevel;
	skillpoints = SkillPoints;
	att = AttributePoints;
	return CharacterStatistic;
}
void UProjectXStatsComponent::StatsInit(int32 level, int32 skillpoints, int32 att, FStatsInfo stats)
{
	CurrentLevel = level;
	SkillPoints = skillpoints;
	AttributePoints = att;
	CharacterStatistic = stats;
}
void UProjectXStatsComponent::BasicCompsInit()
{
	Inventory = Cast<UProjectXInventoryComponent>(GetOwner()->GetComponentByClass(UProjectXInventoryComponent::StaticClass()));
	HealthComponent = Cast<UProjectXHealthComponent>(GetOwner()->GetComponentByClass(UProjectXHealthComponent::StaticClass()));
}
/*  Delete if not used
FStatsInfo UProjectXStatsComponent::GetCharacterInfo(int32& CurrentSkillPoints, int32& CurrentAttributePoints, int32& CharacterLevel)
{
	CharacterLevel = CurrentLevel;
	CurrentAttributePoints = AttributePoints;
	CurrentSkillPoints = SkillPoints;
	return CharacterStatistic;
}
*/
bool UProjectXStatsComponent::RiseStat(EStatTypesName StatName)
{
	bool bIsSucces = false;
	if (UseAttributePoints())
	{			
		switch (StatName)
		{
		case EStatTypesName::Strength:
			CharacterStatistic.Strength = CharacterStatistic.Strength + 1;
			StrRiseResult();
			break;
		case EStatTypesName::Constitution:
			CharacterStatistic.Constitution = CharacterStatistic.Constitution + 1;
			ConRiseResult(false);
			break;
		case EStatTypesName::Intelligence:
			CharacterStatistic.Intelligence = CharacterStatistic.Intelligence + 1;
			break;
		case EStatTypesName::Reaction:
			ReactionRiseResult();
			CharacterStatistic.Reaction = CharacterStatistic.Reaction + 1;
			break;
		default:
			break;
		}
		bIsSucces = true;
		
		OnStatsChange.Broadcast(CharacterStatistic, CurrentLevel, SkillPoints, AttributePoints);
	}
	
	return bIsSucces;
}

