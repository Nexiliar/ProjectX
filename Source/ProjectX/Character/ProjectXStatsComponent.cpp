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
	OnStatsChange.Broadcast(CharacterStatistic, CurrentLevel,AttributePoints);
	//StatDependance();
	// ...

	AmountOfExpirienceNeedForLvlUpPerLevel.SetNum(MaxLevel);
	BasicCompsInit();
	//ConRiseResult(true,0);
	//ReactionRiseResult(true,0);
	//StrRiseResult(true,0);
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
	CurrentExpirience += Amount;
	float CurrentLeveExpirience;
	GetExpInfo(CurrentLeveExpirience);
	 
	if (CurrentExpirience > LevelExpirience)
	{
		float AmountExpLeft = CurrentExpirience - LevelExpirience;
		CurrentExpirience = 0.0f;
		LevelUpEvent();	
		ExpGain(AmountExpLeft);
		
	}
	else if (CurrentExpirience == LevelExpirience)
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
		CurrentLevel += 1;
		
		AttributePoints += 1;
		AvailableAttributePoints +=1;
		MaxAttributePoints +=1;

		HealthComponent->SetCurrentHealth(HealthComponent->GetCurrentMaxHealth());
		Inventory->MaxWeightLimit += 5;
		OnStatsChange.Broadcast(CharacterStatistic, CurrentLevel, AttributePoints);
		OnLevelUp.Broadcast();
	}
	
}

UProjectXHealthComponent* UProjectXStatsComponent::GetHealthComponent()
{
	return HealthComponent;
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
				HealthComponent->AddMaxHealthValue(10);

				if (CharacterStatistic.Constitution >= 10)
				{
					if (!Con10)
					{
						HealthComponent->CoefDamage -= 0.1;
						Con10 = true;
					}
					
				}
				if (CharacterStatistic.Constitution >= 15)
				{
					if (!Con15)
					{
						HealthComponent->CoefDamage -= 0.1;
						HealthComponent->AddMaxHealthValue(30);
						Con15 = true;
					}

				}
				if (CharacterStatistic.Constitution >= 20)
				{
					if (!Con20)
					{
						Con20 = true;
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

			if (CharacterStatistic.Strength >= 10)
			{
				IsSlotUnlocked(1);
			}
			if (CharacterStatistic.Strength >= 15)
			{
				IsSlotUnlocked(2);
			}
			if (CharacterStatistic.Strength >= 20)
			{
				IsSlotUnlocked(3);
				HealthComponent->CoefDamage -= 0.01f;

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
bool UProjectXStatsComponent::IsSlotUnlocked(int32 SlotIndex)
{
	bool bIsUnlocked = false;
	if (!Inventory->WeaponSlotsInfo[SlotIndex].CanUseSlot)
	{
		Inventory->WeaponSlotsInfo[SlotIndex].CanUseSlot = true;
		bIsUnlocked = true;
	}	

	return bIsUnlocked;
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

				if (CharacterStatistic.Reaction >= 10)
				{
					if (!React10)
					{
						Player->BonusReloadSpeed += 0.1f;
						React10 = true;
						
					}
					
				}

				if (CharacterStatistic.Reaction >= 15)
				{
					if (!React15)
					{
						Player->BonusReloadSpeed += 0.2f;
						React15 = true;
						
					}					
				}

				if (CharacterStatistic.Reaction >= 20)
				{
					if (!React20)
					{
						React20 = true;
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
	
	
}
FStatsInfo UProjectXStatsComponent::GetEveryStat(int32& level,int32& att)
{
	level = CurrentLevel;	
	att = AttributePoints;
	return CharacterStatistic;
}
void UProjectXStatsComponent::StatsInit(bool FirstInit, int32 level, int32 att, FStatsInfo stats)
{
	CurrentLevel = level;
	AttributePoints = att;
	CharacterStatistic = stats;
	int32 TempCon;
	int32 TempStr;
	int32 TempReac;

	if (FirstInit)
	{
		TempCon = DefaultCharStatistic.Constitution;
		TempStr = DefaultCharStatistic.Strength;
		TempReac = DefaultCharStatistic.Reaction;
		ConRiseResult(true, TempCon);
		StrRiseResult(true, TempStr);
		ReactionRiseResult(true, TempReac);
	}
	else
	{
		TempCon = CharacterStatistic.Constitution; //-= DefaultCharStatistic.Constitution;
		TempStr = CharacterStatistic.Strength; //-= DefaultCharStatistic.Strength;
		TempReac = CharacterStatistic.Reaction; //-= DefaultCharStatistic.Reaction;
		ConRiseResult(false, TempCon);
		StrRiseResult(false, TempStr);
		ReactionRiseResult(false, TempReac);
	}


	
}
void UProjectXStatsComponent::BasicCompsInit()
{
	Inventory = Cast<UProjectXInventoryComponent>(GetOwner()->GetComponentByClass(UProjectXInventoryComponent::StaticClass()));
	HealthComponent = Cast<UProjectXHealthComponent>(GetOwner()->GetComponentByClass(UProjectXHealthComponent::StaticClass()));
}

void UProjectXStatsComponent::RiseAttValue(FName StatName, int32& CurrentAtt, int32 &CurrentAttributePoints)
{
	if (AvailableAttributePoints > 0 )
	{
		if (StatName == "Str")
		{
			STR += 1;
			CurrentAtt = STR + CharacterStatistic.Strength;
		}
		if (StatName == "Con")
		{
			CON += 1;
			CurrentAtt = CON + CharacterStatistic.Constitution;
		}
		if (StatName == "React")
		{
			REACT += 1;
			CurrentAtt = REACT + CharacterStatistic.Reaction;
		}
		AvailableAttributePoints -= 1;
		CurrentAttributePoints = AvailableAttributePoints;
	}
}

void UProjectXStatsComponent::DecreaseAttValue(FName StatName, int32& CurrentAtt, int32& CurrentAttributePoints)
{
	if (AvailableAttributePoints != MaxAttributePoints)
	{
		if (StatName == "Str")
		{			
			if (STR + CharacterStatistic.Strength > CharacterStatistic.Strength)
			{
				STR -= 1;
				CurrentAtt = STR + CharacterStatistic.Strength;
				AvailableAttributePoints += 1;
			}
			
			
		}
		if (StatName == "Con")
		{
			if (CON + CharacterStatistic.Constitution > CharacterStatistic.Constitution)
			{
				CON -= 1;
				CurrentAtt = CON + CharacterStatistic.Constitution;
				AvailableAttributePoints += 1;
			}
			
		}
		if (StatName == "React")
		{
			if (REACT + CharacterStatistic.Reaction > CharacterStatistic.Reaction)
			{
				REACT -= 1;
				CurrentAtt = REACT + CharacterStatistic.Reaction;
				AvailableAttributePoints += 1;
			}
			
		}	
		
		CurrentAttributePoints = AvailableAttributePoints;
	}
}

void UProjectXStatsComponent::ApplyPoints()
{
	RiseStat(EStatTypesName::Strength, STR);
	STR = 0;
	RiseStat(EStatTypesName::Constitution, CON);
	CON = 0;
	RiseStat(EStatTypesName::Reaction, REACT);
	REACT = 0;
	
	if (AvailableAttributePoints == 0)
	{
		MaxAttributePoints = 0;
	}
	else
	{
		MaxAttributePoints = AvailableAttributePoints;
	}
}

void UProjectXStatsComponent::GetAttMath(int32& AvailableAtt, int32& MaxAtt)
{
	AvailableAtt = AvailableAttributePoints;
	MaxAtt = MaxAttributePoints;
}

void UProjectXStatsComponent::SetAttMath(int32 AvailableAtt, int32 MaxAtt)
{
	AvailableAttributePoints = AvailableAtt;
	MaxAttributePoints = MaxAtt;
}

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
		
		OnStatsChange.Broadcast(CharacterStatistic, CurrentLevel, AttributePoints);
	}
	
	return bIsSucces;
}

