// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectX/Interfaces/ProjectX_Interface_GameActor.h"

// Add default functionality here for any IProjectX_Interface_GameActor functions that are not pure virtual.

EPhysicalSurface IProjectX_Interface_GameActor::GetSurfaceType()
{
	return EPhysicalSurface::SurfaceType_Default;
}

TArray<UProjectX_StateEffect*> IProjectX_Interface_GameActor::GetAllCurrentEffects()
{
	TArray<UProjectX_StateEffect*> Effect;
	
	return Effect;
}

void IProjectX_Interface_GameActor::RemoveEffect(UProjectX_StateEffect* RemoveEffect)
{
}

void IProjectX_Interface_GameActor::AddEffect(UProjectX_StateEffect* NewEffect)
{
}

void IProjectX_Interface_GameActor::GetSpawnLocationForEffect(FVector& MeshLocation, FName& BoneName)
{

}

//void IProjectX_Interface_GameActor::DropItemToWorld(FDropItem DropItemInfo)
//{
	
//}
