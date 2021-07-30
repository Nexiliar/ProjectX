// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectX/Character/ProjectXStatsComponent.h"
#include "ProjectX/Character/ProjectXCharacter.h"



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

	AmountOfExpirienceNeedForLvlUpPerLevel.SetNum(MaxLevel);
	BasicCompsInit();
	ConRiseResult(true,0);
	ReactionRiseResult(true,0);
	StrRiseResult(true,0);
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
		if (AmountOfExpirienceNeedForLvlUpPerLevel[i] != NULL)
		{
			LevelExpirience = AmountOfExpirienceNeedForLvlUpPerLevel[i];
			CurrentMaxExp = LevelExpirience;
		}

	}
	
	return CurrentExpirience;
}



void UProjectXStatsComponent::LevelUpEvent()
{
	if (CurrentLevel == MaxLevel)
	{

	}
	else
	{
		CurrentLevel = CurrentLevel + 1;
		SkillPoints = SkillPoints + 1;
		AttributePoints = AttributePoints + 1;


		Inventory->MaxWeightLimit += 5;
		OnStatsChange.Broadcast(CharacterStatistic, CurrentLevel, SkillPoints, AttributePoints);
		OnLevelUp.Broadcast();
	}
	
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
bool UProjectXStatsComponent::UseAttributePoints(int32 PointsDoDecrease)
{
	bool bIsSuccess = false;
	if (AttributePoints > 0)
	{
		AttributePoints = AttributePoints - PointsDoDecrease;
		bIsSuccess = true;
	}
	return bIsSuccess;
}
void UProjectXStatsComponent::ConRiseResult(bool Init, int32 AmountOfPointstoAdd)
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
		for (int32 i = 0; i < AmountOfPointstoAdd; i++)
		{
			if (HealthComponent)
			{
				//float ToChangeHealthValue = CharacterStatistic.Constitution * 10;
			//	HealthComponent->SetCurrentMaxHealth(ToChangeHealthValue);
				HealthComponent->AddMaxHealthValue(10);

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
					HealthComponent->CoefDamage -= 0.2;
					HealthComponent->AddMaxHealthValue(60);

					UProjectXSkillComponent* Skill = Cast<UProjectXSkillComponent>(GetOwner()->GetComponentByClass(UProjectXSkillComponent::StaticClass()));
					if (Skill)
					{
						Skill->isBastionModeAvailable = true;
						Skill->BonusSkill = ESkillList::BastionMode;
					}
				}
			}
			if (Inventory)
			{
				Inventory->MaxWeightLimit += 2;
				Inventory->OnCurrentWeightChange.Broadcast(Inventory->CurrentWeight, Inventory->MaxWeightLimit);
			}
		}
	}
		

	

}
void UProjectXStatsComponent::StrRiseResult(bool Init,int32 AmountOfPointstoAdd)
{
	if (Init)
	{
		Inventory->MaxWeightLimit += CharacterStatistic.Strength * 2;
	}
	else
	{

	
	for (int32 i = 0; i < AmountOfPointstoAdd; i++)
	{
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

				UProjectXSkillComponent* Skill = Cast<UProjectXSkillComponent>(GetOwner()->GetComponentByClass(UProjectXSkillComponent::StaticClass()));
				if (Skill)
				{
					Skill->isRageModeAvailable = true;

					Skill->BonusSkill = ESkillList::RageMode;

				}
			}
		}
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
void UProjectXStatsComponent::ReactionRiseResult(bool Init,int32 AmountOfPointstoAdd)
{
	
	AProjectXCharacter* Player = Cast<AProjectXCharacter>(GetOwner());
	if (Init)
	{
		if (Player)
		{
			Player->CorrectAccuracyOnStatUp += CharacterStatistic.Reaction * 0.1f;
			Player->CorrectFireRateOnStatUp += CharacterStatistic.Reaction* 0.004f;
		}
	}
	else
	{
		for (int32 i = 0; i < AmountOfPointstoAdd; i++)
		{
			
			if (Player)
			{
				Player->CorrectAccuracyOnStatUp += 0.1f;
				Player->CorrectFireRateOnStatUp += 0.004f;

				if (CharacterStatistic.Reaction == 10)
				{
					Player->BonusReloadSpeed += 0.1f;
				}

				if (CharacterStatistic.Reaction == 15)
				{
					Player->BonusReloadSpeed += 0.2f;
				}

				if (CharacterStatistic.Reaction == 20)
				{
					UProjectXSkillComponent* Skill = Cast<UProjectXSkillComponent>(GetOwner()->GetComponentByClass(UProjectXSkillComponent::StaticClass()));
					if (Skill)
					{
						Skill->isSnakeModeAvailable = true;

						Skill->BonusSkill = ESkillList::SnakeMode;

					}
					Player->BonusReloadSpeed += 0.3f;
				}
			}
		}
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
bool UProjectXStatsComponent::RiseStat(EStatTypesName StatName, int32 AmountOfPoits)
{
	bool bIsSucces = false;
	if (UseAttributePoints(AmountOfPoits))
	{			
		switch (StatName)
		{
		case EStatTypesName::Strength:
			CharacterStatistic.Strength = CharacterStatistic.Strength + AmountOfPoits;
			StrRiseResult(false,AmountOfPoits);
			break;
		case EStatTypesName::Constitution:
			CharacterStatistic.Constitution = CharacterStatistic.Constitution + AmountOfPoits;
			ConRiseResult(false, AmountOfPoits);
			break;
		case EStatTypesName::Intelligence:
			CharacterStatistic.Intelligence = CharacterStatistic.Intelligence + AmountOfPoits;
			break;
		case EStatTypesName::Reaction:
			ReactionRiseResult(false,AmountOfPoits);
			CharacterStatistic.Reaction = CharacterStatistic.Reaction + AmountOfPoits;
			break;
		default:
			break;
		}
		bIsSucces = true;
		
		OnStatsChange.Broadcast(CharacterStatistic, CurrentLevel, SkillPoints, AttributePoints);
	}
	
	return bIsSucces;
}

