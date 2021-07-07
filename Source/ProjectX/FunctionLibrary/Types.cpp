// Fill out your copyright notice in the Description page of Project Settings.


#include "Types.h"
#include "ProjectX/Interfaces/ProjectX_Interface_GameActor.h"
#include "ProjectX/ProjectX.h"

void UTypes::AddEffectBySurfaceType(AActor* TakeEffectActor, TSubclassOf<UProjectX_StateEffect> AddEffectClass, EPhysicalSurface SurfaceType)
{
	if (SurfaceType != EPhysicalSurface::SurfaceType_Default && TakeEffectActor && AddEffectClass)
	{
		UProjectX_StateEffect* myEffect = Cast<UProjectX_StateEffect>(AddEffectClass->GetDefaultObject());
			if (myEffect)
			{
				bool bIsHavePossibleSurface = false;
				int8 j = 0;
				while (j < myEffect->PossibleInteractSurface.Num() && !bIsHavePossibleSurface)
				{
					if (myEffect->PossibleInteractSurface[j] == SurfaceType)
					{	
						bIsHavePossibleSurface = true;
						bool bIsCanAddEffect = false;
						if (!myEffect->bIsStackable)
						{
							int8 i = 0;
							TArray<UProjectX_StateEffect*> CurrentEffects;
							IProjectX_Interface_GameActor* myInterface = Cast<IProjectX_Interface_GameActor>(TakeEffectActor);
							if (myInterface)
							{
								CurrentEffects = myInterface->GetAllCurrentEffects();
							}
							if (CurrentEffects.Num() > 0)
							{
								while (i < CurrentEffects.Num() && !bIsCanAddEffect)
								{
									if (CurrentEffects[i]->GetClass() != AddEffectClass)
									{
										bIsCanAddEffect = true;
									}
									i++;
								}
							}
							else
							{
								bIsCanAddEffect = true;
							}
							
						}
						else
						{
							bIsCanAddEffect = true;
						}


						if (bIsCanAddEffect)
						{
							
							UProjectX_StateEffect* NewEffect = NewObject<UProjectX_StateEffect>(TakeEffectActor, AddEffectClass);
							if (NewEffect)
							{
								NewEffect->InitObject(TakeEffectActor);
							}
						}		
						
					}
					j++;
				}
			}
	}
}
