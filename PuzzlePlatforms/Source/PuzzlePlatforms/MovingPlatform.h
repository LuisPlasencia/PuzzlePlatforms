// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AMovingPlatform();

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float Speed = 20;	

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true)) // para que me aparezca como un gizmo en el editor y así poder poner esta posición más fácil
	FVector TargetLocation;

	void AddActiveTrigger();
	void RemoveActiveTrigger();
	
private:
	FVector GlobalTargetLocation;
	FVector GlobalStartLocation;

	UPROPERTY(EditAnywhere)
	int ActiveTriggers = 1;

};
